#include <d3d11.h>
#include <tchar.h>
#include <Windows.h>

#include "imgui.h"
#include "imgui_freetype.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

#include "framework/settings/functions.h"
#include "framework/data/fonts.h"

static ID3D11Device*           g_pd3dDevice = nullptr;
static ID3D11DeviceContext*    g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*         g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRTV = nullptr;

static bool create_device(HWND hWnd);
static void cleanup_device();
static void create_rtv();
static void cleanup_rtv();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
		{
			cleanup_rtv();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			create_rtv();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

static void load_fonts(ImGuiIO& io)
{
	io.Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
	io.Fonts->FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Monochrome | ImGuiFreeTypeBuilderFlags_MonoHinting;

	ImFontConfig cfg;
	cfg.OversampleH = 3;
	cfg.OversampleV = 3;
	cfg.PixelSnapH = true;
	cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Monochrome | ImGuiFreeTypeBuilderFlags_MonoHinting;
	cfg.FontDataOwnedByAtlas = false;

	const ImWchar* ranges = io.Fonts->GetGlyphRangesCyrillic();

	var->font.icons[0] = io.Fonts->AddFontFromMemoryTTF(section_icons_hex, (int)sizeof(section_icons_hex), 15.0f, &cfg, ranges);
	var->font.icons[1] = io.Fonts->AddFontFromMemoryTTF(icons_hex,         (int)sizeof(icons_hex),          5.0f, &cfg, ranges);
	var->font.tahoma   = io.Fonts->AddFontFromMemoryTTF(tahoma_hex,        (int)sizeof(tahoma_hex),        13.0f, &cfg, ranges);

	ImFontConfig disk_cfg;
	disk_cfg.OversampleH = 3;
	disk_cfg.OversampleV = 3;
	disk_cfg.PixelSnapH = true;
	disk_cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Monochrome | ImGuiFreeTypeBuilderFlags_MonoHinting;

	var->font.fontnew = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\tahomabd.ttf", 13.0f, &disk_cfg, ranges);
	if (!var->font.fontnew)
		var->font.fontnew = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\tahomabd.TTF", 13.0f, &disk_cfg, ranges);
	if (!var->font.fontnew)
	{
		char winroot[MAX_PATH] = {};
		GetWindowsDirectoryA(winroot, MAX_PATH);
		char path[MAX_PATH];
		wsprintfA(path, "%s\\Fonts\\tahomabd.ttf", winroot);
		var->font.fontnew = io.Fonts->AddFontFromFileTTF(path, 13.0f, &disk_cfg, ranges);
	}
	if (!var->font.fontnew)
		var->font.fontnew = var->font.tahoma;

	var->font.visitor = var->font.tahoma;

	if (!var->font.tahoma)
	{
		io.Fonts->AddFontDefault();
		var->font.tahoma = io.Fonts->Fonts[0];
	}
	if (!var->font.icons[0]) var->font.icons[0] = var->font.tahoma;
	if (!var->font.icons[1]) var->font.icons[1] = var->font.tahoma;
}

int main(int, char**)
{
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"atlashook", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"atlashook", WS_OVERLAPPEDWINDOW, 100, 100, 900, 700, nullptr, nullptr, wc.hInstance, nullptr);

	if (!create_device(hwnd))
	{
		cleanup_device();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	load_fonts(io);

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	const float clear_color[4] = { 0.07f, 0.07f, 0.07f, 1.0f };

	bool running = true;
	while (running)
	{
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				running = false;
		}
		if (!running)
			break;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		gui->render();

		ImGui::Render();
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRTV, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRTV, clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	cleanup_device();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);
	return 0;
}

static bool create_device(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (hr == DXGI_ERROR_UNSUPPORTED)
		hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (FAILED(hr))
		return false;

	create_rtv();
	return true;
}

static void cleanup_device()
{
	cleanup_rtv();
	if (g_pSwapChain)        { g_pSwapChain->Release();        g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice)        { g_pd3dDevice->Release();        g_pd3dDevice = nullptr; }
}

static void create_rtv()
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer)
	{
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRTV);
		pBackBuffer->Release();
	}
}

static void cleanup_rtv()
{
	if (g_mainRTV) { g_mainRTV->Release(); g_mainRTV = nullptr; }
}
