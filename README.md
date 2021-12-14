# Runas Redux

![Node.js CI](https://github.com/N-Blade/node-runas-redux/workflows/Node.js%20CI/badge.svg)

Run command synchronously with administrator privilege.

> This is an N-API based rewrite that adds compatibility with modern NodeJS versions.


## Installing

```sh
npm install runas-redux
```

## Building
  * Clone the repository
  * Run `npm install`
  * Run `grunt` to compile the native and CoffeeScript code
  * Run `grunt test` to run the specs

## Docs

```js
const runas = require('runas-redux')
```

### runas(command, args[, options])

* `options` Object
  * `hide` Boolean - Hide the console window, `true` by default.
  * `admin` Boolean - Run command as administrator, `false` by default.
  * `catchOutput` Boolean - Catch the stdout and stderr of the command, `false`
    by default.
  * `stdin` String - String which would be passed as stdin input.

Launches a new process with the given `command`, with command line arguments in
`args`.

This function is synchronous and returns the exit code when the `command`
finished.

When the `catchOutput` option is specified to `true`, an object that contains
`exitCode`, `stdout` and `stderr` will be returned.

## Limitations

* Only windows is supported