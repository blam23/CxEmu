#include "SDL.h"
#include "gbc/emulator.h"
#include "nes/emulator.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <atomic>
#include <exception>
#include <format>
#include <print>
#include <thread>

// Choose the emulator to run
constexpr bool run_gbc{ true };

constexpr bool delay{ true };
constexpr int scale{ 4 };

auto init_sdl() -> bool
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::println("Unable to initialise SDL: {}", SDL_GetError());
        return false;
    }

    return true;
}

auto handle_key(cx::nes::emulator& emulator, SDL_Scancode sc, bool state) -> void
{
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

auto handle_key(cx::gbc::emulator& emulator, SDL_Scancode sc, bool state) -> void
{
    using key = cx::gbc::joypad::key;

    auto set = [&](key k) {
        if (state)
            emulator.m_jp1.press(k);
        else
            emulator.m_jp1.release(k);
    };

    switch (sc)
    {
    case SDL_SCANCODE_W:
        set(key::up);
        break;
    case SDL_SCANCODE_A:
        set(key::left);
        break;
    case SDL_SCANCODE_S:
        set(key::down);
        break;
    case SDL_SCANCODE_D:
        set(key::right);
        break;
    case SDL_SCANCODE_K:
        set(key::a);
        break;
    case SDL_SCANCODE_L:
        set(key::b);
        break;
    case SDL_SCANCODE_RETURN:
        set(key::start);
        break;
    case SDL_SCANCODE_BACKSPACE:
        set(key::select);
        break;
    default:
        break;
    }
}

template <typename emu_t>
auto run(emu_t& emulator, int width, int height) -> int
{
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

    const auto frame_ms{ 1000.0 / emulator.frames_per_second() };

    auto last_frame{ SDL_GetPerformanceCounter() };
    u64 frames{ 0 };
    emulator.m_ppu.set_frame_complete_callback([&](u8* data, size_t size) {
        SDL_memcpy(pixel_surface->pixels, data, size);
        frames++;

        if (delay)
        {
            auto now{ SDL_GetPerformanceCounter() };
            auto delta{ (f64)((now - last_frame) * 1000 / (f64)SDL_GetPerformanceFrequency()) };

            while (delta < frame_ms)
            {
                now = SDL_GetPerformanceCounter();
                delta = (f64)((now - last_frame) * 1000 / (f64)SDL_GetPerformanceFrequency());
            }

            last_frame = now;
        }
    });

    // setup a renderer for just for vsync.
    SDL_Renderer* renderer{ SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) };
    SDL_Texture* dummy_tex{ SDL_CreateTexture(renderer, window_surface->format->format, SDL_TEXTUREACCESS_STREAMING, 1,
                                              1) };
    SDL_Rect dummy_rect{ 0, 0, 1, 1 };

    std::atomic<bool> running{ true };
    std::thread emu_thread{ [&]() {
        try
        {
            while (running)
            {
                u64 cycles{ 0 };
                while (cycles < emulator.cycles_per_frame())
                    cycles += emulator.clock();
            }
        }
        catch (const std::exception& ex)
        {
            std::println("Emulation stopped: {}", ex.what());
            running = false;
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
                handle_key(emulator, evt.key.keysym.scancode, type == SDL_KEYDOWN);
                break;
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

    // Save any state for battery backed carts, etc.
    emulator.shudown();

    return 0;
}

auto main(int argc, char* argv[]) -> int
{
    auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>("async", "output.txt");
#ifndef NDEBUG
    async_file->set_level(spdlog::level::trace);
#endif

    if (!init_sdl())
        return 1;

    cx::nes::mapper::register_factory(255, nullptr);

    int result{ 0 };

    try
    {
        if constexpr (run_gbc)
        {
            cx::gbc::emulator emulator{ cx::gbc::emulator::load_rom(argc > 1 ? argv[1] : "D:/Test/GBC/pokeblue.gb") };
            result = run(emulator, cx::gbc::emulator::SCREEN_WIDTH, cx::gbc::emulator::SCREEN_HEIGHT);
        }
        else
        {
            cx::nes::emulator emulator{ cx::nes::emulator::load_rom(argc > 1 ? argv[1] : "D:/Test/NES/AccuracyCoin.nes") };
            result = run(emulator, 256, 240);
        }
    }
    catch (const std::exception& ex)
    {
        std::println("Error: {}", ex.what());
        result = 2;
    }

    SDL_Quit();

    spdlog::shutdown();

    return result;
}
