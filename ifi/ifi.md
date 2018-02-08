## General

Commands that are synchronous, may generate an immediate result returned through the emitter. So following `eval` of a synchronous command, the following json received is deemed to be the result of _that_ operations and not just any random output.

Whether or not a command returns a json result will depend on the command itself, but it is consistent.

## setEmitter

must be called first.

## emitterFn

receives json. on engine thread. When received, the engine thread waits until `release` is called.

supply context, zero terminated (json) string

## eval

pass json properties and send commands to to engine. 

* `command: "whatever"` asynchronous.
* `save: "filepath"` synchronous.
* `load: "filepath"` synchronous.

Set these before `start`;

* `configdir: "path"`
* `datadir: "path"`
* `story: "name"`

### Requestors

* `location: true`
* `map: true`
* `picture: true`
* `items: true`  // inventory or sidebar content
* `people: true`
* `meta: true`  // game meta data, product info etc.
* `moves: true`
* `values: true` // request updates KV pairs (if supported)

### Reply General

* `text: "text"`
* `channel: 0`
* `picture: "filepath"`
* `picture: {picture}`  // alternative object version 
* `title: "text"`
* `score:` int
* `scoremax:` int
* `items: [{item}...]`  // inventory
* `people: [{person}...]` // people met
* `meta: {meta}`
* `location:` ID 
* `exits:` int // mask
* `map: {map}`
* `moves:` int
* `values: {"key":value,...}`  // KV pairs idea

### Picture

* `name: "filepath"`
* `brightness: 0.0`  // these optional
* `contrast: 0.0`
* `saturation: 0.0`
* `lightness: 0.0`
* `gamma: 1.0`

### Item 

* `name: "label"`
* `id:` ID
* `worn:` bool          // these optional
* `icon: "filepath"`

### Person

* `name: "label"`
* `id:` ID
* `icon: "filepath"`  // these optional

### Map

* `places: [{place}...]`  
* `backimage: "filepath"` // these optional

### Place

* `name: "placename"`
* `id:` ID
* `exits: []` IDs
* `gx:` int
* `gy:` int
* `dark:` bool  // these optioal
* `up:` bool
* `down:` bool
* `items: []` IDs

### Meta

* `title: "name"`
* `author: "name"`
* `covertext: "text"`
* `covertext: {text}` // alternative
* `credits: ""`   // these optional
* `android_market: ""`
* `ios_market: ""`
* `coverimage: "filepath"`
* `covereffect: "name"`  // of shader code

### Text

* `text: "string"`
* `font: "name"`        // these optional
* `fontweight: int`
* `textcol: int`



## release

called by host UI after each receipt of `emitterFn`.

## sync

Wait `timeoutms` for engine to stop. return `true` is engine stopped, in which case engine is now waiting for `release`. Return `false` if timeout reached and engine not ready.


