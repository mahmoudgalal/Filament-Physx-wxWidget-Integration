
#pragma warning(disable : 4996)
#pragma warning(disable : 4146)

#include<iostream>
#include "FilamentApp.h"
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#include"input_key_codes.h"
#endif

#define wxCUSTOM_DEFAULT_FRAME_STYLE \
            (wxMINIMIZE_BOX | \
             wxCLOSE_BOX | \
             wxCAPTION | \
             wxCLIP_CHILDREN)

auto KeyMapper = [](wxKeyCode code)->KeyCode {
    switch (code)
    {
    case WXK_SPACE:
        return KeyCode::SPACE_KEY;
    case WXK_RETURN:
        return KeyCode::RETURN_KEY;
    case WXK_F1:
        return KeyCode::F1_KEY;
    case WXK_F2:
        return KeyCode::F2_KEY;
    case WXK_F3:
        return KeyCode::F3_KEY;
    case WXK_UP:
        return KeyCode::UP_KEY;
    case WXK_DOWN:
        return KeyCode::DOWN_KEY;
    case WXK_LEFT:
        return KeyCode::LEFT_KEY;
    case WXK_RIGHT:
        return KeyCode::RIGHT_KEY;
    case WXK_ESCAPE:
        return KeyCode::ESCAPE_KEY;

    default:
        return KeyCode::NOP;
    }
};

class RenderTimer : public wxTimer
{
public:
    RenderTimer(FilamentApp* app);
    void Notify();
    void start();
private:
    FilamentApp* app;
    long long startTime;
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    FilamentApp* app;
    RenderTimer* timer;
    ~MyApp() {
        delete timer;
        timer = nullptr;
    }
};

class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    void OnClose(wxCloseEvent& event);
    wxDECLARE_EVENT_TABLE();
};

wxIMPLEMENT_APP(MyApp);
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_CLOSE(MyFrame::OnClose)
wxEND_EVENT_TABLE()

bool MyApp::OnInit()
{
#ifdef _DEBUG
#ifdef WIN32
if (AllocConsole())
    {
        FILE* fi = 0;
        freopen_s(&fi, "CONOUT$", "w", stdout);
    }
#endif // WIN32  
#endif // _DEBUG  
    MyFrame* frame = new MyFrame();
    frame->Show(true);

    app = &(FilamentApp::get());
    auto nativeWindow = frame->GetHWND();
    auto size = frame->GetClientSize();
    auto h = size.GetHeight();
    auto w = size.GetWidth();
    app->run((void*)nativeWindow, nullptr, w, h);
    timer = new RenderTimer(app);
    timer->start();
    
    auto borderHeight = wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_BORDER_Y, NULL);
    auto titleHeight = wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_CAPTION_Y,  NULL);
    std::cout << "Window Title height = " << titleHeight<<std::endl;
    std::cout << "Window Title Border height = " << borderHeight << std::endl;
    std::cout << "Window Client size:" << w << "X" << h << std::endl;
    return true;
}

MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "Filament-Nvidia-Physx Integration Demo",wxDefaultPosition,wxSize(1200,800), wxCUSTOM_DEFAULT_FRAME_STYLE)
{
    Bind(wxEVT_LEFT_DOWN, [](wxMouseEvent& event) {
        wxGetApp().app->onMouseDown(event.GetX(), event.GetY());
        });
    Bind(wxEVT_LEFT_UP, [](wxMouseEvent& event) {
        wxGetApp().app->onMouseUp(event.GetX(), event.GetY());
        });
    Bind(wxEVT_MOTION, [](wxMouseEvent& event) {
        wxGetApp().app->onMouseMove(event.GetX(), event.GetY());
        });
    Bind(wxEVT_RIGHT_DOWN, [](wxMouseEvent& event) {
        auto x = event.GetX(); auto y = event.GetY();
        wxGetApp().app->onMouseDown(x,y,FilamentApp::MouseButton::Right_Button);
        });
    Bind(wxEVT_RIGHT_UP, [](wxMouseEvent& event) {
        wxGetApp().app->onMouseUp(event.GetX(), event.GetY(), 
            FilamentApp::MouseButton::Right_Button);
        });
	Bind(wxEVT_KEY_DOWN, [](wxKeyEvent& event) {
		wxKeyCode code = (wxKeyCode)event.GetKeyCode();
		auto key = KeyMapper(code);
		wxGetApp().app->onKeyDown(key);
        if(code == WXK_ESCAPE)
            wxGetApp().Exit();
		});
}

void MyFrame::OnClose(wxCloseEvent& event) {
    wxGetApp().timer->Stop();
    Destroy();    
}

RenderTimer::RenderTimer(FilamentApp* app) : wxTimer()
{
    this->app = app;
    startTime = wxGetLocalTimeMillis().GetValue() ;
}

void RenderTimer::Notify()
{
    if (this->IsRunning()) {
        auto time = wxGetLocalTimeMillis().GetValue();
        app->render(time - startTime);
    }
}

void RenderTimer::start()
{
    wxTimer::Start(1000/60);//60 FPS
}
