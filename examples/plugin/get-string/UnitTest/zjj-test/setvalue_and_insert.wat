;; SPDX-License-Identifier: Apache-2.0
;; SPDX-FileCopyrightText: 2019-2022 Second State INC

(module
  (import "module_wasm_name" "get_string" (func $get_string (param i32 i32 i32 i32)))
  (import "wasi_snapshot_preview1" "fd_write" (func $fd_write (param i32 i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "proc_exit" (func $proc_exit (param i32)))
  (import "wasi_snapshot_preview1" "string_store" (func $string_store(param i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "string_load" (func $string_load(param i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "string_set_value" (func $string_set_value (param i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "string_insert" (func $string_insert(param i32 i32 i32 i32 i32 i32) (result i32)))
  (memory 1 1)
  (func (export "_start")
    (call $get_string (i32.const 24) (i32.const 64) (i32.const 0) (i32.const 20))
    (drop(call $string_store (i32.const 24) (i32.load(i32.const 20)) (i32.const 0)))

    (call $get_string (i32.const 48) (i32.const 64) (i32.const 1) (i32.const 16))
    (drop(call $string_store (i32.const 48) (i32.load(i32.const 16)) (i32.const 0)))

    (drop(call $string_insert (i32.const 24) (i32.load(i32.const 20)) (i32.const 48) (i32.load (i32.const 16)) (i32.const 2) (i32.const 72)))
    (i32.store (i32.const 0) (i32.const 72))
    (i32.store (i32.const 4) (i32.const 50))

    (drop (call $fd_write (i32.const 1) (i32.const 0) (i32.const 1) (i32.const 8)))
    (i32.store (i32.const 72) (i32.const 10))
    (i32.store (i32.const 4) (i32.const 1))
    (drop (call $fd_write (i32.const 1) (i32.const 0) (i32.const 1) (i32.const 8)))

    (drop (call $string_set_value (i32.const 48) (i32.load(i32.const 16)) (i32.const 0)))
;;
    (drop (call $string_load (i32.const 0) (i32.const 24) (i32.const 4)))
    (i32.store (i32.const 0) (i32.const 24))
;;
    (drop (call $fd_write (i32.const 1) (i32.const 0) (i32.const 1) (i32.const 8)))

    (call $proc_exit (i32.const 0))
;;    (drop)
  )
)