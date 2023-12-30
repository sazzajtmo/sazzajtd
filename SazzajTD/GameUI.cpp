#include "GameUI.h"
#include <format>
#include <cstdarg>
#include <iostream>

#include "SDL.h"
#include "SDL_main.h"

#define NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_renderer.h"

#include "GameManager.h"
#include "GameScoreManager.h"

cGameUI* cGameUI::s_instance(nullptr);

static nk_context* g_ctx;

cGameUI::cGameUI()
{
}

cGameUI::~cGameUI()
{
}

cGameUI* cGameUI::GetInstance()
{
	if( !s_instance )
		s_instance = new cGameUI();

	return nullptr;
}

void cGameUI::DestroyInstance()
{
	if (s_instance)
	{
        s_instance->Cleanup();
		delete s_instance;
		s_instance = nullptr;
	}
}

void cGameUI::Init( SDL_Window* sdlWindow, SDL_Renderer* sdlRenderer )
{
    /* scale the renderer output for High-DPI displays */
    float font_scale = 1.f;

    {
        int render_w, render_h;
        int window_w, window_h;
        float scale_x, scale_y;
        SDL_GetRendererOutputSize(sdlRenderer, &render_w, &render_h);
        SDL_GetWindowSize(sdlWindow, &window_w, &window_h);
        scale_x = (float)(render_w) / (float)(window_w);
        scale_y = (float)(render_h) / (float)(window_h);
        SDL_RenderSetScale(sdlRenderer, scale_x, scale_y);
        font_scale = scale_y;
    }

    g_ctx = nk_sdl_init( sdlWindow, sdlRenderer );

    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {
        struct nk_font_atlas* atlas;
        struct nk_font_config config = nk_font_config(0);
        struct nk_font* font;

        /* set up the font atlas and add desired font; note that font sizes are
         * multiplied by font_scale to produce better results at higher DPIs */
        nk_sdl_font_stash_begin(&atlas);
        font = nk_font_atlas_add_default(atlas, 16 * font_scale, &config);
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10 * font_scale, &config);*/
        /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13 * font_scale, &config);*/
        nk_sdl_font_stash_end();

        /* this hack makes the font appear to be scaled down to the desired
         * size and is only necessary when font_scale > 1 */
        font->handle.height /= font_scale;
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        nk_style_set_font(g_ctx, &font->handle);
    }

    if(0)
    {
        nk_color table[NK_COLOR_COUNT];
        table[NK_COLOR_TEXT] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_WINDOW] = nk_rgba(57, 67, 71, 54);
        table[NK_COLOR_HEADER] = nk_rgba(51, 51, 56, 220);
        table[NK_COLOR_BORDER] = nk_rgba(46, 46, 46, 255);
        table[NK_COLOR_BUTTON] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_BUTTON_HOVER] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(63, 98, 126, 255);
        table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
        table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
        table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
        table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_PROPERTY] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
        table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
        table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
        table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
        table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
        table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
        nk_style_from_table(g_ctx, table);
    }
}

void cGameUI::Cleanup()
{
    nk_sdl_shutdown();
}

void cGameUI::InputBegin()
{
    nk_input_begin(g_ctx);
}

void cGameUI::InputHandle(SDL_Event* sdlEvent)
{
    nk_sdl_handle_event(sdlEvent);
}

void cGameUI::InputEnd()
{
    nk_input_end(g_ctx);
}

void cGameUI::Update(float deltaTime)
{
    float totalGameScore = cGameManager::GetInstance()->GetScoreManager()->GetScore();

    nk_colorf bg;
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

    if (nk_begin(g_ctx, "Score & Stuff", nk_rect(0, 0, 200, 30), NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_static(g_ctx, 1, 80, 2);
        nk_layout_row_static(g_ctx, 10, 80, 2);
        nk_label(g_ctx, "Score", NK_TEXT_LEFT);
        nk_label(g_ctx, std::format("{:g}", totalGameScore).c_str(), NK_TEXT_LEFT);

        //enum { EASY, HARD };
        //static int op = EASY;
        //static int property = 20;

        //nk_layout_row_static(g_ctx, 30, 80, 1);
        //if (nk_button_label(g_ctx, "button"))
        //    fprintf(stdout, "button pressed\n");
        //nk_layout_row_dynamic(g_ctx, 30, 2);
        //if (nk_option_label(g_ctx, "easy", op == EASY)) op = EASY;
        //if (nk_option_label(g_ctx, "hard", op == HARD)) op = HARD;
        //nk_layout_row_dynamic(g_ctx, 25, 1);
        //nk_property_int(g_ctx, "Compression:", 0, &property, 100, 10, 1);

        //nk_layout_row_dynamic(g_ctx, 20, 1);
        //nk_label(g_ctx, "background:", NK_TEXT_LEFT);
        //nk_layout_row_dynamic(g_ctx, 25, 1);
        //if (nk_combo_begin_color(g_ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(g_ctx), 400))) {
        //    nk_layout_row_dynamic(g_ctx, 120, 1);
        //    bg = nk_color_picker(g_ctx, bg, NK_RGBA);
        //    nk_layout_row_dynamic(g_ctx, 25, 1);
        //    bg.r = nk_propertyf(g_ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
        //    bg.g = nk_propertyf(g_ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
        //    bg.b = nk_propertyf(g_ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
        //    bg.a = nk_propertyf(g_ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
        //    nk_combo_end(g_ctx);
        //}
    }
    nk_end(g_ctx);
}

void cGameUI::Draw()
{
    nk_sdl_render(NK_ANTI_ALIASING_ON);
}
