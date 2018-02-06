## General

Commands that are synchronous, may generate an immediate result returned through the emitter. So following `eval` of a synchronous command, the following json received is deemed to be the result of _that_ operations and not just any random output.

Whether or not a command returns a json result will depend on the command itself, but it is consistent.

## setEmitter

must be called first.

## eval

pass json properties and send commands to to engine. 

* `command: "whatever"` asynchronous.
* `save: "filepath"` synchronous.
* `load: "filepath"` synchronous.

Set these before `start`;

* `configDir: "path"`
* `dataDir: "path"`
* `story: "name"`


## emitterFn

receives json. on engine thread. When received, the engine thread waits until `release` is called.

supply context, zero terminated (json) string

* `text: "text"`
* `channel: 0`
* `picture: "filepath"`


## release

called by host UI after each receipt of `emitterFn`.

## sync

Wait `timeoutms` for engine to stop. return `true` is engine stopped, in which case engine is now waiting for `release`. Return `false` if timeout reached and engine not ready.


