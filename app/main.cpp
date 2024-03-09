#include "SDL.h"
#include "nes/emulator.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <print>
#include <thread>

constexpr int scale{ 4 };
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
    // async_file->set_level(spdlog::level::trace);

    if (!init_sdl())
        return 1;

    cx::nes::mapper::register_factory(255, nullptr);

    cx::nes::emulator emulator{ cx::nes::emulator::load_rom("D:/Test/NES/nestest.nes") };

    SDL_Window* window = SDL_CreateWindow("First program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          scale * width, scale * height, SDL_WINDOW_OPENGL);
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

    bool running{ true };
    std::thread emu_thread{ [&]() {
        while (running)
        {
            for (size_t i = 0; i < emulator.cycles_per_frame(); i++)
            {
                emulator.clock();
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
        // draw
        SDL_LowerBlitScaled(pixel_surface, &pixel_rect, window_surface, &window_rect);
        SDL_UpdateWindowSurface(window);

        const auto now{ SDL_GetPerformanceCounter() };
        counter += (f64)((now - prev) * 1000 / (f64)SDL_GetPerformanceFrequency());
        prev = now;

        if (counter > 1000.0)
        {
            std::println("FPS: {}", frames);
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
