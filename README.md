The most up to date repository can be found [here](https://git.konelinx.be/tei.git), including the development branch.

# Tei

🥕 A small game engine written in C++

This engine is mostly based around SDL 2 and makes use of various C++20 features.

## Client

### Entry point

`main` should not be defined by the client.

When the engine is ready, it will call the client implemented function `void TeiClientInit()`.
After the quit signal has been received and the engine is cleaning itself up, the user implemented `void TeiClientQuit()` is called.

It is important the function return when the client has loaded its resources. After completion, the engine will start its inner game loop.

An example of these function:
```c++
// main.cpp

void TeiClientInit()
{
  puts("My game starting...");
  MyGame::Load();
}

void TeiClientQuit()
{
  puts("My game cleaning up...");
  MyGame::Cleanup();
}
```

All statics, services and resources provided and managed by the engine will only be detroyed after `void TeiClientQuit()` has been called.

## Objects and Components

### `Object`

`Object` is a container that is attached to a single parent Object but can have multiple children Objects. It can also be given `Components`.
This essentially forms the tree graph through which the game is updated and rendered.

Objects cannot be created without a parent.
To add a new child to a Object, use `AddChild()`:
```c++
auto& child = parent.AddChild();
```
The child object is correctly attached to the parent and is ready to be used.

When an Object is enabled or disabled, it will tray in its state during the current frame. As of the next frame, it will have changed.
Enabling or disabling an Object will cause its children to also be enabled or disabled.

An object's children or component list cannot be modified while it is active.

### Components

Components will house the bulk of your logic and are the main powerhouses of your game. They may implement `OnInitialize` and `OnCleanup`, `OnEnable` and `OnDisable`, and `OnUpdate`, `OnFixedUpdate` and `OnRender`.

Through external polymorphism any type in C++ can be a component. None of the implmentable callbacks are required.

Any callback may be a free function or a member function with the following syntax, exemplified by OnUpdate:
```c++
component.OnEnable(parent);
component.OnEnable();
::OnEnable(component, parent);
::OnEnable(component);
```
Only one of these is called. Lookup happens in the order of the examples.

A component does not have to be trivial, copyable or movable.

Adding component to an Object:
```c++
auto& myTimer = object.AddComponent<MyTimer>(); // Default constructed
auto& myTimer = object.AddComponent<MyTimer>(1, 2, 3); // Constructed inplace
auto& myTimer = object.AddComponent(MyTimer{}); // Moved in
```

There is also a way to add multiple components.
```c++
// Both components are moved in, in order left to right.
auto [myTimer, myData] = object.AddComponents(MyTimer{ 27 }, MyData{ "world" });
```
The return type can be destructured.

To get a component, use `GetComponent`. This returns a reference and throws if the component is not present.
Use `HasComponent` to get a pointer, which will be a nullptr if the component is not present.

To remove a component, use `RemoveComponent`. If the component is movable, it can be received.

### Update sequence

The update will propagate in a depth first manner, starting from the top.
An object's components will be updated before its children will.
Components added first will be updated first.

### Clearing

To clear up an object and remove its children, `Clear()` can be called on a disabled object.
Alternatively, `ClearChildren()` will only remove the children.

## Services

### Scenes

This is the root of the object tree. `tei::Scenes` allows scenes to be added and switched between. Scenes are equivilant to a GameObject without a parent.

### Time

global time and frame information is provided by the `tei::Time` static.
Types and functions can be found in the `tei::Clock` class.

`Clock::time_point` and `Clock::duration` hold their values in seconds. Their epoch is the start of the program.

```c++
  // Time point the frame is currently in.
  tei::Time->frame.now
  // Duration of the last frame, or the fixed time step of the game loop.
  tei::Time->frame.delta
```

Literals are also provided: days `_d`, hours `_h`, minutes `_m`, seconds `_s`, milliseconds `_ms`, microseconds `_us`, nanoseconds `_ns`.

```c++
  // Constructs a Clock::duration of 100 milliseconds
  if (tei::Time->frame.delta > 100_ms)
```

### Resources

Resources can be loaded through `tei::Resources`. Some engine loadable resources can be found under `tei::resources`. Custom types can also be provided.

To load a resource and make sure it does not get loaded twice. Use `LoadShared`.
To force a resource to load regardles of previous copies, Use `LoadUnique`.

The resource wrapper functions equivalent to shared pointers, when they are copied, their resource only exists once.

Some resources may be initialised without loading them; E.g. a default texture is an empty one with size 0x0.

To load a shared resource:
```c++
  // Loads the sprite.png
  auto gameSprite{ tei::Resources->LoadShared<tei::resources::Texture>("./resources/sprite.png") };
```

### Renderer and Audio

Textures and sprites may be rendered through `tei::Renderer` and audio may be played through `tei::Audio`.

Some rendering logic is provided by `tei::components::TextureRenderComponent`, `tei::components::SpriteRenderComponent` and `tei::components::TextRenderComponent`.

### Events and messages

Global events can de dispatched through `tei::Event`.

In a closed environment, `tei::components::Subject` is provided. To make use of this, it can be added as a component or may be inherited from to include the interface.

To dispatch an event or message:
```c++
subject.Notify(MyEvent{ PLAYER_DIED });
```

Messages may be of any type and will be dispatched to observers listening for them.
To add an observer
```c++
subject.AddObserver(
  [] (MyEvent event)
  {
    // Something to do
  }
);
```
This example uses a lambda. Observers may also be a functor with a call operator or a function pointer. There is no support for member functions. Functors may hold state and be mutable.

This returns an observer handle. When it is destroyed or overwritten, it will remove the observer from the subject again. So make sure to keep track of it as long as the link must be kept alive. A handle may also be detached permanently leaving it in the subject.

### Input

Input can be interacted with via `tei::Input`. It behaves similar to `tei::Event`, but has additional functionality.

And additional parameter to AddCommand is the key or axis to listen for.
```c++
subject.AddObserver(
  tei::input::ControllerInput::Trigger::LEFT,
  [] (InputAnalog::Data input)
  {
    // Something to do
  }
);
```

### Application

The application can be accessed through `tei::Application`. Commandline arguments can also be found here.

To close the application: call `tei::Application->Quit()`. This will initiate cleanup.

Window properties can also be modified through Application.

## Other

### Internals

It is not recommended to ever use `tei::internal`. It holds the internal logic.

# てゐ

<img src="tewi_w.png" alt="てゐ" >
