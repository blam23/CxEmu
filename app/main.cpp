#include "SDL.h"
#include "nes/emulator.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <print>
#include <thread>

constexpr bool delay{ false };
constexpr int scale{ 2 };
constexpr int width{ 256 };
constexpr int height{ 240 };

auto init_sdl() -> bool
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::println("Unable to initialise SDL: {}", SDL_GetError());
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>("async", "output.txt");
#ifndef NDEBUG
    async_file->set_level(spdlog::level::trace);
#endif

    if (!init_sdl())
        return 1;

    cx::nes::mapper::register_factory(255, nullptr);

    cx::nes::emulator emulator{ cx::nes::emulator::load_rom("D:/Test/NES/nestest.nes") };

    SDL_Window* window = SDL_CreateWindow("CxEmu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale * width,
                                          scale * height, SDL_WINDOW_OPENGL);
    if (window == NULL)
    {
        std::println("Unable to create SDL window: {}", SDL_GetError());
        return 3;
    }
    SDL_Surface* pixel_surface{ SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_BGR24) };
    SDL_Surface* window_surface{ SDL_GetWindowSurface(window) };

    SDL_Rect pixel_rect{ 0, 0, width, height };
    SDL_Rect window_rect{ 0, 0, width * scale, height * scale };

    u64 frames{ 0 };
    emulator.m_ppu.set_frame_complete_callback([pixel_surface, &frames](u8* data, size_t size) {
        SDL_memcpy(pixel_surface->pixels, data, size);
        frames++;
    });

    // setup a renderer for just for vsync.
    SDL_Renderer* renderer{ SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) };
    SDL_Texture* dummy_tex{ SDL_CreateTexture(renderer, window_surface->format->format, SDL_TEXTUREACCESS_STREAMING, 1,
                                              1) };
    SDL_Rect dummy_rect{ 0, 0, 1, 1 };

    bool running{ true };
    std::thread emu_thread{ [&]() {
        auto last_frame{ SDL_GetPerformanceCounter() };

        while (running)
        {
            u64 cycles{ 0 };
            while (cycles < emulator.cycles_per_frame())
                cycles += emulator.clock();

            if (delay)
            {
                const auto now{ SDL_GetPerformanceCounter() };
                auto delta{ (f64)((now - last_frame) * 1000 / (f64)SDL_GetPerformanceFrequency()) };

                while (delta < 64)
                {
                    const auto now{ SDL_GetPerformanceCounter() };
                    delta = (f64)((now - last_frame) * 1000 / (f64)SDL_GetPerformanceFrequency());
                }

                last_frame = now;
            }
        }
    } };

    auto counter{ 0.0 };
    auto prev{ SDL_GetPerformanceCounter() };
    while (running)
    {
        SDL_Event evt;
        while (SDL_PollEvent(&evt))
        {
            const auto type{ evt.type };
            switch (type)
            {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                const auto state{ type == SDL_KEYDOWN };
                const auto sc{ evt.key.keysym.scancode };

                switch (sc)
                {
                case SDL_SCANCODE_W:
                    emulator.m_jp1.m_status.flags.up = state;
                    break;
                case SDL_SCANCODE_A:
                    emulator.m_jp1.m_status.flags.left = state;
                    break;
                case SDL_SCANCODE_S:
                    emulator.m_jp1.m_status.flags.down = state;
                    break;
                case SDL_SCANCODE_D:
                    emulator.m_jp1.m_status.flags.right = state;
                    break;
                case SDL_SCANCODE_K:
                    emulator.m_jp1.m_status.flags.a = state;
                    break;
                case SDL_SCANCODE_L:
                    emulator.m_jp1.m_status.flags.b = state;
                    break;
                case SDL_SCANCODE_RETURN:
                    emulator.m_jp1.m_status.flags.start = state;
                    break;
                case SDL_SCANCODE_BACKSPACE:
                    emulator.m_jp1.m_status.flags.select = state;
                    break;
                default:
                    break;
                }
            }
        }

        // draw to window surface
        SDL_LowerBlitScaled(pixel_surface, &pixel_rect, window_surface, &window_rect);

        // force vsync
        SDL_RenderCopy(renderer, dummy_tex, nullptr, &dummy_rect);
        SDL_RenderPresent(renderer);

        // actually update window
        SDL_UpdateWindowSurface(window);

        const auto now{ SDL_GetPerformanceCounter() };
        counter += (f64)((now - prev) * 1000 / (f64)SDL_GetPerformanceFrequency());
        prev = now;

        if (counter > 1000.0)
        {
            SDL_SetWindowTitle(window, std::format("FPS: {}", frames).c_str());
            frames = 0;
            counter = 0.0;
        }
    }

    running = false;
    emu_thread.join();

    SDL_Quit();

    spdlog::shutdown();

    return 0;
}
