;; SPDX-License-Identifier: Apache-2.0
;; SPDX-FileCopyrightText: 2019-2022 Second State INC

(module
  (import "module_wasm_name" "get_string" (func $get_string (param i32 i32 i32 i32)))
  (import "wasi_snapshot_preview1" "fd_write" (func $fd_write (param i32 i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "proc_exit" (func $proc_exit (param i32)))
  (import "wasi_snapshot_preview1" "string_store" (func $string_store(param i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "string_load" (func $string_load(param i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "string_from_int" (func $string_from_int(param i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "string_concat" (func $string_concat (param i32 i32 i32 i32 i32 i32) (result i32)))
  (memory 1 1)
  (func (export "_start")
    ;; address layout
    ;; 8 -> fd_write result
    ;; 16 -> ciovec array with one element
    ;;   16 -> pointer to buffer
    ;;   20 -> buffer size
    ;; 24 -> buffer with size 64
    ;; call get_string
    ;; 24 -> pointer to buffer
    ;; 64 -> buffer size
;;     20 -> pointer to written
    (call $get_string (i32.const 24) (i32.const 24) (i32.const 0) (i32.const 10))
    (call $get_string (i32.const 48) (i32.const 24) (i32.const 1) (i32.const 20))


    (drop (call $string_concat (i32.const 24) (i32.load (i32.const 10)) (i32.const 48) (i32.load (i32.const 20)) (i32.const 80) (i32.const 74)))

    (drop (call $string_from_int (i32.load (i32.const 74)) (i32.const 0) (i32.const 8)))
    ;; set up ciovec
    (i32.store (i32.const 16) (i32.const 80))
    ;; set up ciovec len
    (i32.store (i32.const 20) (i32.const 26))
    ;; add \n in the end of string
;;    i32.const 8
;;    i32.const 8
;;    i32.load
;;    i32.const 4
;;    i32.add
;;    i32.store
;;    (i32.store (i32.load(i32.const 8)) (i32.const 10))

    (drop (call $fd_write (i32.const 1) (i32.const 16) (i32.const 1) (i32.const 8)))

    ;; call proc_exit
    ;; 0 -> successful exit
    (call $proc_exit (i32.const 0))
  )
)