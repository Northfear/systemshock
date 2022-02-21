/*

Copyright (C) 2015-2018 Night Dive Studios, LLC.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
//====================================================================================
//
//		System Shock - ©1994-1995 Looking Glass Technologies, Inc.
//
//		Shock.c	-	Mac-specific initialization and main event loop.
//
//====================================================================================

//--------------------
//  Includes
//--------------------
#include <math.h>
#include <SDL.h>

#include "InitMac.h"
#include "Modding.h"
#include "OpenGL.h"
#include "Prefs.h"
#include "Shock.h"
#include "ShockBitmap.h"

#include "amaploop.h"
#include "gr2ss.h"
#include "hkeyfunc.h"
#include "mainloop.h"
#include "setup.h"
#include "shockolate_version.h"
#include "status.h"
#include "version.h"

#ifdef VITA
#include <psp2/kernel/clib.h>
#include <psp2/power.h>
#include <vita2d.h>
#include <unistd.h>

int _newlib_heap_size_user = 256 * 1024 * 1024;

enum
{
    VITA_FULLSCREEN_WIDTH = 960,
    VITA_FULLSCREEN_HEIGHT = 544,
};

SDL_Surface *surface = NULL;
vita2d_texture *texBuffer;
uint8_t *palettedTexturePointer;
SDL_Rect destRect;
SDL_GameController *gameController;
SDL_Sensor *vitaGyro;

void *memcpy(void *destination, const void *source, size_t n)
{
	return sceClibMemcpy(destination, source, n);
}

void *memset(void *destination, int c, size_t n)
{
	return sceClibMemset(destination, c, n);
}

void *memmove(void *destination, const void *source, size_t n)
{
	return sceClibMemmove(destination, source, n);
}

int memcmp(const void *arr1, const void *arr2, size_t n)
{
	return sceClibMemcmp(arr1, arr2, n);
}
#endif

//--------------------
//  Globals
//--------------------
bool gPlayingGame;

grs_screen *cit_screen;
SDL_Window *window;
SDL_Palette *sdlPalette;
SDL_Renderer *renderer;

SDL_AudioDeviceID device;

int num_args;
char **arg_values;

extern grs_screen *svga_screen;
extern frc *svga_render_context;

//--------------------
//  Prototypes
//--------------------
extern void init_all(void);
extern void inv_change_fullscreen(uchar on);
extern void object_data_flush(void);
extern errtype load_da_palette(void);

// see Prefs.c
extern void CreateDefaultKeybindsFile(void);
extern void LoadHotkeyKeybinds(void);
extern void LoadMoveKeybinds(void);

#ifdef VITA
void OpenController()
{
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            gameController = SDL_GameControllerOpen(i);
        }
    }
}

void CloseController()
{
    if (SDL_GameControllerGetAttached(gameController)) {
        SDL_GameControllerClose(gameController);
        gameController = NULL;
    }
}

void OpenGyro()
{
    for (int i = 0; i < SDL_NumSensors(); ++i) {
        if (SDL_SensorGetDeviceType(i) == SDL_SENSOR_GYRO) {
            vitaGyro = SDL_SensorOpen(i);
        }
    }
}

void SetRenderRect(int width, int height)
{
    // screen scaling calculation
    destRect.x = 0;
    destRect.y = 0;
    destRect.w = width;
    destRect.h = height;

    int isFullScreen = 1;

    if ( width != VITA_FULLSCREEN_WIDTH || height != VITA_FULLSCREEN_HEIGHT ) {
        if ( isFullScreen ) {
            //vita2d_texture_set_filters(texBuffer, SCE_GXM_TEXTURE_FILTER_LINEAR, SCE_GXM_TEXTURE_FILTER_LINEAR);
            if (((float)( VITA_FULLSCREEN_WIDTH ) / VITA_FULLSCREEN_HEIGHT ) >= ((float)( width ) / height ) ) {
                const float scale = (float)( VITA_FULLSCREEN_HEIGHT ) / height;
                destRect.w = (int32_t)((float)( width ) * scale );
                destRect.h = VITA_FULLSCREEN_HEIGHT;
                destRect.x = ( VITA_FULLSCREEN_WIDTH - destRect.w ) / 2;
            }
            else {
                const float scale = (float)( VITA_FULLSCREEN_WIDTH ) / width;
                destRect.w = VITA_FULLSCREEN_WIDTH;
                destRect.h = (int32_t)( (float)( height ) * scale );
                destRect.y = ( VITA_FULLSCREEN_HEIGHT - destRect.h ) / 2;
            }
        }
        else {
            // center game area
            destRect.x = ( VITA_FULLSCREEN_WIDTH - width ) / 2;
            destRect.y = ( VITA_FULLSCREEN_HEIGHT - height ) / 2;
        }
    }
}
#endif

//------------------------------------------------------------------------------------
//		Main function.
//------------------------------------------------------------------------------------
int main(int argc, char **argv) {
#ifdef VITA
    if (chdir(VITA_PATH) != 0)
    {
        sceClibPrintf("Unable to chdir!\n");
        return 1;
    }

	scePowerSetArmClockFrequency(444);
	scePowerSetBusClockFrequency(222);
	scePowerSetGpuClockFrequency(222);
	scePowerSetGpuXbarClockFrequency(166);
#endif
    // Save the arguments for later

    num_args = argc;
    arg_values = argv;

    // FIXME externalize this
    log_set_quiet(0);
    log_set_level(LOG_INFO);

    INFO("Logger initialized");

    // init mac managers

    InitMac();

    // Initialize the preferences file.

    SetDefaultPrefs();
    LoadPrefs();

    // see Prefs.c
    CreateDefaultKeybindsFile(); // only if it doesn't already exist
    // even if keybinds file still doesn't exist, defaults will be set here
    LoadHotkeyKeybinds();
    LoadMoveKeybinds();

    // Process some startup arguments

    bool show_splash = !CheckArgument("-nosplash");

    // CC: Modding support! This is so exciting.

    ProcessModArgs(argc, argv);

    // Initialize

    init_all();
    setup_init();

    gPlayingGame = true;

    load_da_palette();
    gr_clear(0xFF);

    // Draw the splash screen

    INFO("Showing splash screen");
    splash_draw(show_splash);

    // Start in the Main Menu loop

    _new_mode = _current_loop = SETUP_LOOP;
    loopmode_enter(SETUP_LOOP);

    // Start the main loop

    INFO("Showing main menu, starting game loop");
    mainloop(argc, argv);

    status_bio_end();
    stop_music();

    return 0;
}

bool CheckArgument(char *arg) {
    if (arg == NULL)
        return false;

    for (int i = 1; i < num_args; i++) {
        if (strcmp(arg_values[i], arg) == 0) {
            return true;
        }
    }

    return false;
}

#ifdef VITA2D
void InitVita2D(int width, int height)
{
    vita2d_init();

    window = SDL_CreateWindow("", 0, 0, width, height, 0);

    vita2d_texture_set_alloc_memblock_type( SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW );
    texBuffer = vita2d_create_empty_texture_format(width, height, SCE_GXM_TEXTURE_FORMAT_P8_ABGR);
    palettedTexturePointer = (uint8_t*)(vita2d_texture_get_datap(texBuffer));
    memset(palettedTexturePointer, 0, width * height * sizeof(uint8_t));

    SetRenderRect(width, height);
}

void ClearVita2D()
{
    if (window != NULL) {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    vita2d_fini();

    if (texBuffer != NULL) {
        vita2d_free_texture(texBuffer);
        texBuffer = NULL;
    }
}
#endif

void InitSDL() {
#ifdef VITA2D
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_SENSOR) < 0) {
        DEBUG("%s: Init failed", __FUNCTION__);
    }

    gr_init();

    extern short svga_mode_data[];
    gr_set_mode(svga_mode_data[gShockPrefs.doVideoMode], TRUE);

    INFO("Setting up screen and render contexts");

    // Create a canvas to draw to
    SetupOffscreenBitmaps(grd_cap->w, grd_cap->h);

    InitVita2D(grd_cap->w, grd_cap->h);

    OpenController();
    OpenGyro();

    // Create the palette
    sdlPalette = SDL_AllocPalette(256);

    // Setup the screen
    svga_screen = cit_screen = gr_alloc_screen(grd_cap->w, grd_cap->h);
    gr_set_screen(svga_screen);

    gr_alloc_ipal();

    atexit(SDL_Quit);

    SDLDraw();
#else
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
        DEBUG("%s: Init failed", __FUNCTION__);
    }

    // TODO: figure out some universal set of settings that work...
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

    gr_init();

    extern short svga_mode_data[];
    gr_set_mode(svga_mode_data[gShockPrefs.doVideoMode], TRUE);

    INFO("Setting up screen and render contexts");

    // Create a canvas to draw to

    SetupOffscreenBitmaps(grd_cap->w, grd_cap->h);

    // Open our window!
    char window_title[128];
    sprintf(window_title, "System Shock - %s", SHOCKOLATE_VERSION);

#ifdef VITA
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, grd_cap->w, grd_cap->h,
                              SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_SENSOR);
    SetRenderRect(grd_cap->w, grd_cap->h);
    OpenController();
    OpenGyro();
#else
    window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, grd_cap->w, grd_cap->h,
                              SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
#endif

    // Create the palette

    sdlPalette = SDL_AllocPalette(256);

    // Setup the screen

    svga_screen = cit_screen = gr_alloc_screen(grd_cap->w, grd_cap->h);
    gr_set_screen(svga_screen);

    gr_alloc_ipal();

    SDL_ShowCursor(SDL_DISABLE);

    atexit(SDL_Quit);

    SDL_RaiseWindow(window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, grd_cap->w, grd_cap->h);

    // Startup OpenGL

    init_opengl();

    SDLDraw();

    SDL_ShowWindow(window);
#endif
}

SDL_Color gamePalette[256];
bool UseCutscenePalette = FALSE; // see cutsloop.c
void SetSDLPalette(int index, int count, uchar *pal) {
    static bool gammalut_init = 0;
    static uchar gammalut[100 - 10 + 1][256];
    if (!gammalut_init) {
        double factor = (can_use_opengl() ? 1.0 : 2.2); // OpenGL uses 2.2
        int i, j;
        for (i = 10; i <= 100; i++) {
            double gamma = (double)i * 1.0 / 100;
            gamma = 1 - gamma;
            gamma *= gamma;
            gamma = 1 - gamma;
            gamma = 1 / (gamma * factor);
            for (j = 0; j < 256; j++)
                gammalut[i - 10][j] = (uchar)(pow((double)j / 255, gamma) * 255);
        }
        gammalut_init = 1;
        INFO("Gamma LUT init\'ed");
    }

    int gam = gShockPrefs.doGamma;
    if (gam < 10)
        gam = 10;
    if (gam > 100)
        gam = 100;
    gam -= 10;

    for (int i = index; i < index + count; i++) {
        gamePalette[i].r = gammalut[gam][*pal++];
        gamePalette[i].g = gammalut[gam][*pal++];
        gamePalette[i].b = gammalut[gam][*pal++];
        gamePalette[i].a = 0xff;
    }

    if (!UseCutscenePalette) {
        // Hack black!
        gamePalette[255].r = 0x0;
        gamePalette[255].g = 0x0;
        gamePalette[255].b = 0x0;
        gamePalette[255].a = 0xff;
    }

    SDL_SetPaletteColors(sdlPalette, gamePalette, 0, 256);
    SDL_SetSurfacePalette(drawSurface, sdlPalette);
    SDL_SetSurfacePalette(offscreenDrawSurface, sdlPalette);

    if (should_opengl_swap())
        opengl_change_palette();
#ifdef VITA2D
    uint32_t palette32Bit[256u];

    if (!surface) {
        surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    }

    for ( size_t i = 0; i < 256u; ++i ) {
        palette32Bit[i] = SDL_MapRGBA(surface->format, gamePalette[i].r, gamePalette[i].g, gamePalette[i].b, gamePalette[i].a);
    }

    memcpy(vita2d_texture_get_palette(texBuffer), palette32Bit, sizeof(uint32_t) * 256);
#endif
}

void SDLDraw() {
#ifdef VITA2D
    SDL_memcpy(palettedTexturePointer, drawSurface->pixels, gScreenWide * gScreenHigh * sizeof(uint8_t));

    vita2d_start_drawing();

    vita2d_draw_rectangle(0, 0, VITA_FULLSCREEN_WIDTH, VITA_FULLSCREEN_HEIGHT, 0xff000000);
    vita2d_draw_texture_scale(texBuffer, destRect.x, destRect.y, (float)(destRect.w) / gScreenWide,
                                (float)(destRect.h) / gScreenHigh);
    vita2d_end_drawing();
    vita2d_common_dialog_update();
    vita2d_swap_buffers();
#else
    if (should_opengl_swap()) {
        sdlPalette->colors[255].a = 0x00;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, drawSurface);

    if (should_opengl_swap()) {
        sdlPalette->colors[255].a = 0xff;
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }

    SDL_Rect srcRect = {0, 0, gScreenWide, gScreenHigh};
#ifdef VITA
    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
#else
    SDL_RenderCopy(renderer, texture, &srcRect, NULL);
#endif
    SDL_DestroyTexture(texture);

    if (should_opengl_swap()) {
        opengl_swap_and_restore();
    } else {
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
    }
#endif
}

bool MouseCaptured = FALSE;

extern int mlook_enabled;

void CaptureMouse(bool capture) {
    MouseCaptured = (capture && gShockPrefs.goCaptureMouse);

    if (!MouseCaptured && mlook_enabled && SDL_GetRelativeMouseMode() == SDL_TRUE) {
        SDL_SetRelativeMouseMode(SDL_FALSE);

        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        SDL_WarpMouseInWindow(window, w / 2, h / 2);
    } else
        SDL_SetRelativeMouseMode(MouseCaptured ? SDL_TRUE : SDL_FALSE);
}
