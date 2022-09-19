
#pragma warning(disable : 4996)
#pragma warning(disable : 4146)

#include<iostream>
#include "FilamentApp.h"
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#define wxCUSTOM_DEFAULT_FRAME_STYLE \
            (wxMINIMIZE_BOX | \
             wxCLOSE_BOX | \
             wxCAPTION | \
             wxCLIP_CHILDREN)

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
    auto size = frame->GetSize();
    auto h = size.GetHeight();
    auto w = size.GetWidth();
    app->run((void*)nativeWindow, nullptr, w, h);
    timer = new RenderTimer(app);
    timer->start();
    return true;
}

MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "Filament-Nvidia-Physx Integration Demo",wxDefaultPosition,wxSize(1024,800), wxCUSTOM_DEFAULT_FRAME_STYLE)
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
	Bind(wxEVT_KEY_DOWN, [](wxKeyEvent& event) {
		if (event.GetKeyCode() == WXK_SPACE) {
			wxGetApp().app->createEyeProjectile();
		}
		else if (event.GetKeyCode() == WXK_RETURN)
		{
			wxGetApp().app->createEyeProjectile(FilamentApp::CubeProjectile);
		}
        else if (event.GetKeyCode() == WXK_F1)
        {
            wxGetApp().app->createPyramidStack(5);
        }
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
