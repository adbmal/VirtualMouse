
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

    void Execute (const AsyncProgressWorker::ExecutionProgress& progress) {
        m_progress = &progress;
        for (int i = 0; i < 2; ++i) {
            progress.Send(reinterpret_cast<const char*>(&i), sizeof(int));
        }
        screen->run();
    }

    virtual void onShapeUpdate(int x, int y, int w, int h) {

    }
    virtual void onMouseMove(float x, float y) {
        float data[2] = {x, y};
        m_progress->Send(reinterpret_cast<const char*>(&data[0]), 2*sizeof(float));
    }

    void HandleProgressCallback(const char *data, size_t count) {
        HandleScope scope;
        float *position = reinterpret_cast<float*>(const_cast<char*>(data));

        v8::Local<v8::Value> argv[] = {
            New<v8::Number>(position[0]),
            New<v8::Number>(position[1])
        };
        m_mouseMove->Call(2, argv, async_resource);
    }

private:
    const AsyncProgressWorker::ExecutionProgress *m_progress;
    Callback *m_updateShape;
    Callback *m_mouseMove;
};

NAN_METHOD(getShape) {
    int x, y, w, h;
    if (screen) {
        screen->getShape(x, y, w, h);
        Local<Array> shape = New<v8::Array>(4);
        Nan::Set(shape, 0, Nan::New(x));
        Nan::Set(shape, 1, Nan::New(y));
        Nan::Set(shape, 2, Nan::New(w));
        Nan::Set(shape, 3, Nan::New(h));
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

NAN_MODULE_INIT(InitAll) {
    screen = IScreen::getScreen();

    Set(target, New<String>("getShape").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(getShape)).ToLocalChecked());

    Set(target, New<v8::String>("start").ToLocalChecked(),
        New<v8::FunctionTemplate>(start)->GetFunction());
    Set(target, New<v8::String>("stop").ToLocalChecked(),
        New<v8::FunctionTemplate>(stop)->GetFunction());

}

NODE_MODULE(screen, InitAll)