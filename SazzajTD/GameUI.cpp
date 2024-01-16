#include "GameUI.h"
#include <format>
#include <cstdarg>
#include <iostream>
#include <filesystem>
#include <functional>

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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GameManager.h"
#include "GameScoreManager.h"
#include "GameRenderer.h"

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
		s_instance = snew cGameUI();

	return s_instance;
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
    //m_uiStates = 
    //{
    //    { eGameState::StartUp, []( float deltaTime ){ cGameUI::GetInstance()->UpdateStartUp(deltaTime); } },
    //    { eGameState::Playing, [](float deltaTime) { cGameUI::GetInstance()->UpdatePlaying(deltaTime); } },
    //    { eGameState::Paused, [](float deltaTime) { cGameUI::GetInstance()->UpdatePaused(deltaTime); } }
    //};

    /* scale the renderer output for High-DPI displays */
    float font_scale = 1.f;

    {
        int render_w, render_h;
        int window_w, window_h;
        float scale_x, scale_y;
        SDL_GetRendererOutputSize(sdlRenderer, &m_renderW, &m_renderH);
        SDL_GetRendererOutputSize(sdlRenderer, &render_w, &render_h);
        SDL_GetWindowSize(sdlWindow, &window_w, &window_h);
        scale_x = (float)(m_renderW) / (float)(window_w);
        scale_y = (float)(m_renderH) / (float)(window_h);
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

    //auto buttonStyle = g_ctx->style.button;
    //buttonStyle.padding.y += 4.f;
    //buttonStyle.border += 4.f;
    //g_ctx->style.button = buttonStyle;

    static std::vector<std::filesystem::path> imagePaths = 
    {
        "ui/kenneyAck.png",
        "ui/gogoLogo.png",
        "buildings/building_slow.png",
        "buildings/building_gun.png"
    };

    static int nkImgID = 0;
    nkImgID++;

    for (const auto& path : imagePaths)
    {
        std::string imageName = path.filename().string();
        SDL_Texture* sdlTex = cGameRenderer::GetInstance()->GetSDLTexture(path.string().c_str());
        int imgW, imgH;
        SDL_QueryTexture(sdlTex, nullptr, nullptr, &imgW, &imgH);

        std::unique_ptr<struct nk_image> nkImgPtr = std::make_unique<struct nk_image>();
        nkImgPtr->handle.ptr = (void*)sdlTex;
        nkImgPtr->region[0] = 0;
        nkImgPtr->region[1] = 0;
        nkImgPtr->region[2] = imgW;
        nkImgPtr->region[3] = imgH;
        nkImgPtr->w = imgW;
        nkImgPtr->h = imgH;
        m_images[imageName] = std::move(nkImgPtr);
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
    static std::unordered_map<eGameState, std::function<void(float)>> uiStates =
    {
        { eGameState::StartUp, [](float deltaTime) { cGameUI::GetInstance()->UpdateStartUp(deltaTime); } },
        { eGameState::Playing, [](float deltaTime) { cGameUI::GetInstance()->UpdatePlaying(deltaTime); } },
        { eGameState::Paused, [](float deltaTime) { cGameUI::GetInstance()->UpdatePaused(deltaTime); } }
    };

    const eGameState gameState = cGameManager::GetInstance()->GetGameState();

    uiStates[gameState]( deltaTime );
}

void cGameUI::Draw()
{
    nk_sdl_render(NK_ANTI_ALIASING_ON);
}

void cGameUI::UpdateStartUp(float deltaTime)
{
    static const float menuColsRatio[] = { 0.2f, 0.6f, 0.2f };

    if (nk_begin(g_ctx, "Start Menu", nk_rect(0, 0, m_renderW, m_renderH), 0))
    {
        nk_layout_row_static( g_ctx, m_renderH / 2, 15, 1 );

        nk_layout_row( g_ctx, NK_DYNAMIC, 40, 3, menuColsRatio );

        nk_spacing( g_ctx, 1 );
        
        if (nk_button_label(g_ctx, "Start"))
        {
            cGameManager::GetInstance()->SetGameState( eGameState::Playing );
        }

        nk_spacing(g_ctx, 1);

        nk_spacing(g_ctx, 1);

        if (nk_button_label(g_ctx, "Start Paused"))
        {
            cGameManager::GetInstance()->SetGameState(eGameState::Paused);
        }

        nk_spacing(g_ctx, 1);

        const auto& kenneyLogo = m_images["kenneyAck.png"];
        nk_draw_image(&g_ctx->current->buffer, nk_rect(m_renderW - kenneyLogo->w - 20, m_renderH - kenneyLogo->h, kenneyLogo->w, kenneyLogo->h), kenneyLogo.get(), nk_white);

        const auto& logo = m_images["gogoLogo.png"];
        nk_draw_image(&g_ctx->current->buffer, nk_rect( ( m_renderW - logo->w ) / 2, m_renderH / 2 - logo->h, logo->w, logo->h), logo.get(), nk_white);
    }

    nk_end(g_ctx);
}

void cGameUI::UpdatePaused(float deltaTime)
{
    UpdatePlaying( deltaTime );
}

void cGameUI::UpdatePlaying(float deltaTime)
{
    eGameState  gameState           = cGameManager::GetInstance()->GetGameState();
    float       totalGameScore      = cGameManager::GetInstance()->GetScoreManager()->GetScore();
    int         numBuildingsLeft    = cGameManager::GetInstance()->GetNumAvailableBuildings();

    if (nk_begin(g_ctx, "Menu", nk_rect(0, 0, m_renderW, 30), NK_WINDOW_NO_SCROLLBAR))
    {
        //nk_layout_row_static(g_ctx, 22, 80, 7);
        nk_layout_row_dynamic(g_ctx, 22, 8);
        nk_label(g_ctx, "Score :", NK_TEXT_LEFT);
        nk_label(g_ctx, std::format("{:g}", totalGameScore).c_str(), NK_TEXT_LEFT);
        nk_label(g_ctx, "# Blds :", NK_TEXT_LEFT);
        nk_label(g_ctx, std::format("{:d}", numBuildingsLeft).c_str(), NK_TEXT_LEFT);
        nk_label(g_ctx, "Next :", NK_TEXT_LEFT);

        if (numBuildingsLeft > 0)
        {            
            constexpr int numBuildingTypes = static_cast<int>(eGameObjectTypes::Buildings_End) + static_cast<int>(eGameObjectTypes::Buildings_Start) + 1;

            static const std::array<std::string, numBuildingTypes> buildingIcons =
            {
                "building_slow.png",
                "building_gun.png"
            };

            eGameObjectTypes nextBuildingType = cGameManager::GetConstInstance()->GetCurrentBuildingType();
            int buildingIconIdx = std::clamp( static_cast<int>(nextBuildingType) - static_cast<int>(eGameObjectTypes::Buildings_Start), 0, numBuildingTypes );

            const auto& buildingIcon = m_images[buildingIcons[buildingIconIdx]];
            nk_draw_image(&g_ctx->current->buffer, nk_rect(386, 0, buildingIcon->w * 1.5, buildingIcon->h * 1.5), buildingIcon.get(), nk_white);
        }

        nk_spacing(g_ctx, 1);

        if (nk_button_label(g_ctx, "Reset"))
        {
            cGameManager::GetInstance()->Reset();
        }

        if (nk_button_label(g_ctx, gameState == eGameState::Playing ? "Pause" : "Resume" ))
        {
            cGameManager::GetInstance()->SetGameState( gameState == eGameState::Playing ? eGameState::Paused : eGameState::Playing );
        }
    }
    nk_end(g_ctx);
}
