# IFI - Interactive Fiction Interface

_IFI_ is the point of connection between a GUI and an IF _back-end_, _engine_ or _interpreter_ (whatever you prefer to call it).

The purpose of _IFI_ is to convey instructions from the GUI to the _back-end_ and to convey elements of the world state from the _back-end_ to the GUI. These conveyances encode information according to an _implementation independent_ schema.

Specifically, messages in both directions are `json` strings whose schema and term meanings are detailed below. Consequently, the interface itself, needed only for passing strings is especially terse. These few functions are the interface between the GUI and the IF back-end;

```
struct IFI
{
    typedef std::function<void(const char*)> Emitter;

    virtual ~IFI() {}
    static IFI* create();
    
    virtual void setEmitter(const Emitter&) = 0;
    virtual bool eval(const char* json) = 0;
    virtual bool start(int argc, char** argv) = 0;

    virtual bool sync(int timeoutms = 500) = 0;
    virtual void release() = 0;

};
```

In practice, an adapter called `IFIClient` is linked with your back-end to make your interpreter IFI compatible. `IFIClient` contains a lot of helper functions (json parsing etc.). There's also a "C" interface (called `ifiglue`) that allows, an otherwise console interpreter, to become an IFIClient simply by redefining stdio function calls (such as `getchar`, `putchar` etc.).

## Requests and Replies

The back-end (`IFIClient`) receives json _requests_ together with game commands. The back-end then replies by sending a _reply_ json, this reply contains the answers to data requested and also text result from the game command.

`IFI::setEmitter` is called by the front-end to the back-end to give the back-end the `Emitter` function through which to reply. The back-end will use `IFIClient` as "glue" to help the connection.

The back-end can call `IFIClient::getRequest()` to **block** on the next request from the front-end. This will return the next json request. Additionally the back-end can call `IFIClient::getchar()` which will **block** until the next character of game command is returned.

NOTE that the `getRequest` json will contain the `command:`, so that `getchar` is not necessary (but convenient). If `getRequest` is called, a subsequent `getchar` will not block (essentially returning characters of the `command:` already obtained by `getRequest`).

## Requests

These are the json tags that may appear in a _request_. See the _Replies_ section for corresponding reply data to each request.

* `configdir: "path"`  
  Directory where game assets are stored. This directory is not necessarily writable.

* `datadir: "path"`  
   Directory where user specific data is stored. eg save games.
   
* `story: "name"`  
   Prefix name of the story. Files `story`.???  will be in `configdir`

* `objects: true`  
   Request the game objects map. 

* `command: "get lamp"`  
   Text typed by the player, or constructed by the GUI.

* `loaddata: "game-state"`  
   The entire game state is sent to restore a game position. The string "game-state" is _entirely_ back-end specific, for example base64 encoded binary data, whose content and meaning is only known to the back-end. This will include any internal headers, formatting, version numbers etc.

   The "game-state" string will correspond _exactly_ to one previously sent through `savedata` earlier.

* `savedata: true`  
   Request entire game state.

* `location: true`  
   Return `location` reply if `true`. If `false`, unsolicited updates are not needed.
   
* `map: true`  
   Return `map` data reply if `true`. If `false`, unsolicited updates are not needed.
   
* `picture: true`  
   Reply details of current `picture` if `true`. If `false`, unsolicited updates are not needed.
   
* `items: true`  
   Reply current `items` (eg inventory) if `true`. If `false`, unsolicited updates are not needed.
   
* `people: true`  
   Reply current `people` if `true`. If `false`, unsolicited updates are not needed.
   
* `meta: true`  
   Reply the game meta info (eg title, author etc.).
   
* `moves: true`  
   Reply the current game move count (or game time) if `true`. If `false`, unsolicited updates are not needed.

* `values: true`  
   Reply any game specific key-value pairs for use in extensions and to be incorporated in save games.

## Replies

The _reply_ json, sent from the back-end to the front-end, can have these terms at the top level. Some of the terms have values that are (optionally) json objects. In these cases, consult the subsequent json object definitions.

Game objects are associated with an _ID_, which is an arbitrary `int`. The front end makes no assumptions about the meaning of this ID, except that equal IDs mean identical objects.

* `text: "You are in a maze of twisty passages all alike."`  
   Block of text from the game to be formatted and shown in the transcript window. A newline will be added to the end when displayed. The `text` may contain a subset of [Markdown](https://daringfireball.net/projects/markdown/) and HTML.

* `title: "In the Lounge"`  
  Game text to be displayed in any GUI title bar.
   
* `picture: "filepath"`  
   Path relative to `datadir` for picture file (.jpg,.png) to be displayed.
   
* `picture: {pictureobj}`  
   Version of `picture` with more details. NB: either use this form or the above.

* `objects: [{object}...]`  
   The objects in the game. This is primarily the _invariant_ mapping between ID and game name.

* `score:` int  
   Current game store (this may be displayed on the title bar).
   
* `scoremax:` int  
   _Optional_. Max game score. When provided we can either display score/maxscore or score%, otherwise just the score is given.

*  `savedata: "game-state"`  
   The entire game state encoded as a string (eg base64) that can later be accommodated by a `loaddata` request to completely restore a position (see also request `loaddata`).

* `items: [{item}...]`  
   item details for the sidebar (eg inventory).
   
* `people: [{person}...]`  
   _Optional_. People details for the sidebar. Usually this will be people you've met.
   
* `location:` ID    
   ID of current location.
   
* `exits:` int  
   Bitmask of currently available directional exits. This data are used to drive the GUI compass. Canonical game exits follow the following predefined order; N,NE,E,SE,S,SW,W,NW,Up,Down,In,Out. The bitmask is constructed with bit 0 (LSB) being "N", and so on.

* `map: {mapobj}`  
   Data for the currently known map.
   
* `moves:` int  
  Current move count (or game time).

* `meta: {metaobj}`  
   Game meta data. This will only be requested once, usually at the start.

* `values: {key:value,...}`

### pictureobj

* `name: "filepath"`  
  path relative to `datadir` for image file.
  
* `brightness: 0.0`  
  _Optional_. 0.0-1.0. Applied to picture dynamically.
  
* `contrast: 0.0`  
  _Optional_. 0.0-1.0. Applied to picture dynamically.
  
* `saturation: 0.0`  
  _Optional_. 0.0-1.0. Applied to picture dynamically.
 
* `lightness: 0.0`  
  _Optional_. 0.0-1.0. Applied to picture dynamically.
  
* `gamma: 1.0`  
  _Optional_. Applied to picture dynamically.

### object

* `id:` ID
* `name: "game name"`  
   The _game name_ is a string that the game parser will accept to identify the object. For example, if there are several _keys_ in the game "key" is insufficient, the _game name_ must therefore be "small key", "brass key" etc. as appropriate.

### item 

* `id:` ID

* `name: "label"`  
  The text displayed for the item (usually in the sidebar). This can be a _friendly_ name rather than a name that will parse. For example, "A small dog asleep", or "Dr. Livingstone (you presume)". This name can change, whereas the ID _game name_ cannot.
  
* `worn:` bool  
  _Optional_. If provided, the GUI will indicate the item is worn (usually in the sidebar).
  
* `icon: "filepath"`  
  _Optional_. Icon to display in sidebar for item. This is usually a `.SVG` file. If a filepath is given, it is assumed relative to `configdir`. Alternatively, a resource name can be given.

### person

Same meanings as `item`.

* `id:` ID
* `name: "label"`
* `icon: "filepath"` 

### mapobj

* `places: [{place}...]`  
   Array of known places.
   
* `backimage: "filepath"`  
  _Optional_. background image for map page, relative to `configdir`.

### place

* `id:` ID

* `name: "placename"`  
  Text to be displayed in map location box.
  
* `exits: [ID...]`  
   Current exit locations from this place.

* `gx:` int 
   X grid coordinate. Place locations are set on an integer X-Y grid. Imagine all locations are points on this grid and (currently) are all the same size. The map renderer will automatically figure the scale and relative offsets for the map and its consequent plot coordinates.
   
* `gy:` int  
  Y grid coordinate.
  
* `dark: true`  
  _Optional_. Map location is _dark_. If set this will draw the map box in a darker shade.
  
* `up:` bool  
  _Optional_. Indicate _Up_ arrow on map box.

* `down:` bool  
  _Optional_. Indicate _Down_ arrow on map box.
  
* `items: [ID...]`  
  _Optional_. Specify IDs of items in mapbox. This can be used to show the location of _dropped_ items on the map.

### metaobj

* `title: "name"`
* `author: "name"`
* `covertext: "text"`
* `covertext: {text}` 
* `credits: "by Larry Biggs"`

* `android_market: ""`  
   _Optional_. Google Play marketplace link.
   
* `ios_market: ""`  
  _Optional_. Apple Store marketplace link.
  
* `coverimage: "filepath"`  
   Image to display on game cover, path relative to `configdir`.

* `covereffect: "filepath"`  
   _Optional_. Filename containing shader to display on cover, path relative to `configdir`.
   

### text

* `text: "string"`

* `font: "name"`  
  _Optional.
  
* `fontweight: int`  
  _Optional.
  
* `textcol: int`  
  _Optional.

## Interface Functions

### bool start(int argc, char** argv)

Called after `setEmitter` but before any `eval`. Tells the back-end to perform startup and initialisation. When linked through `ifiglue`, `start` will wind up calling into the original console `main`.

Arguments passed are from the original invocation (if any).

Additionally, the argument `-e <json>` will be included in order to send the back-end necessary information it needs to start up. In theory, this can be _any_ allowable _request_ json, but in practice it will be limited to the following terms;

* `configdir`
* `datadir`
* `story`

See the _requests_ section for description of these terms.



### sync

Wait `timeoutms` for engine to stop. return `true` is engine stopped, in which case engine is now waiting for `release`. Return `false` if timeout reached and engine not ready.

### release

Release back-end to execution after `sync`.

