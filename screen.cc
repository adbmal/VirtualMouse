
#include <nan.h>
#include "IScreen.h"  // NOLINT(build/include)

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using v8::Local;
using v8::Array;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

using namespace Nan;

static IScreen *screen = NULL;

class ScreenHook : public AsyncProgressWorker, public ScreenHandle {
public:
    ScreenHook(
        Callback *callback
        , Callback *updateShape
        , Callback *mouseMove)
        : AsyncProgressWorker(callback)
        , m_updateShape(updateShape)
        , m_mouseMove(mouseMove){

    }
    ~ScreenHook() {
        delete m_updateShape;
        delete m_mouseMove;
    }
    typedef struct CallbackEvent {
        enum EventType { Shape, MouseMove} event;
        float x, y;
        int width, height;
    } CallbackEvent;

    void Execute (const AsyncProgressWorker::ExecutionProgress& progress) {
        m_progress = &progress;
        screen->run();
    }

    virtual void onShapeUpdate(int w, int h) {
        CallbackEvent e = {CallbackEvent::Shape, 0, 0, w, h};
        m_progress->Send(reinterpret_cast<const char*>(&e), sizeof(e));
    }
    virtual void onMouseMove(float x, float y) {
        CallbackEvent e = {CallbackEvent::MouseMove, x, y, 0, 0};
        m_progress->Send(reinterpret_cast<const char*>(&e), sizeof(e));
    }

    void HandleProgressCallback(const char *data, size_t count) {
        HandleScope scope;
        CallbackEvent *e = reinterpret_cast<CallbackEvent*>(const_cast<char*>(data));
        switch (e->event) {
        case CallbackEvent::Shape:
        {
            v8::Local<v8::Value> argv[] = {
                New<v8::Uint32>(e->width),
                New<v8::Uint32>(e->height)
            };
            m_mouseMove->Call(2, argv, async_resource);
            break;
        }
        case CallbackEvent::MouseMove:
        {
            v8::Local<v8::Value> argv[] = {
                New<v8::Number>(e->x),
                New<v8::Number>(e->y)
            };
            m_mouseMove->Call(2, argv, async_resource);
            break;
        }
        default:
            break;
        }
    }

private:
    const AsyncProgressWorker::ExecutionProgress *m_progress;
    Callback *m_updateShape;
    Callback *m_mouseMove;
};

NAN_METHOD(getShape) {
    int w, h;
    if (screen) {
        screen->getShape(w, h);
        Local<Object> shape = New<Object>();
        Set(shape, New<String>("width").ToLocalChecked(),Nan::New(w));
        Set(shape, New<String>("height").ToLocalChecked(),Nan::New(h));
        info.GetReturnValue().Set(shape); 
    }
}

NAN_METHOD(start) {
  Callback *updateShape = new Callback(To<v8::Function>(info[0]).ToLocalChecked());
  Callback *mouseMove = new Callback(To<v8::Function>(info[1]).ToLocalChecked());
  Callback *callback = new Callback(To<v8::Function>(info[2]).ToLocalChecked());
  ScreenHook *hook = new ScreenHook(callback, updateShape, mouseMove);
  screen->setHandle(hook);
  AsyncQueueWorker(hook);
}

NAN_METHOD(stop) {
    if (screen) {
        screen->stop();
    }
}

NAN_METHOD(setCursorVisible) {
    if (screen) {
        bool show = info[0]->BooleanValue();
        screen->setCursorVisible(show);
    }
}

NAN_METHOD(warpCursor) {
    if (screen) {
        int x = info[0]->Int32Value();
        int y = info[1]->Int32Value();
        screen->warpCursor(x, y);
    }
}

NAN_MODULE_INIT(InitAll) {
    screen = IScreen::getScreen();
    Set(target, New<String>("getShape").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(getShape)).ToLocalChecked());
    Set(target, New<v8::String>("start").ToLocalChecked(),
        New<v8::FunctionTemplate>(start)->GetFunction());
    Set(target, New<v8::String>("stop").ToLocalChecked(),
        New<v8::FunctionTemplate>(stop)->GetFunction());
    Set(target, New<v8::String>("setCursorVisible").ToLocalChecked(),
        New<v8::FunctionTemplate>(setCursorVisible)->GetFunction());
    Set(target, New<v8::String>("warpCursor").ToLocalChecked(),
        New<v8::FunctionTemplate>(warpCursor)->GetFunction());

}

NODE_MODULE(screen, InitAll)