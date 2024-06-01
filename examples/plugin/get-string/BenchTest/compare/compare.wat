;; SPDX-License-Identifier: Apache-2.0
;; SPDX-FileCopyrightText: 2019-2022 Second State INC

(module
  (import "wasi_snapshot_preview1" "fd_write" (func $fd_write (param i32 i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "proc_exit" (func $proc_exit (param i32)))
  (import "wasi_snapshot_preview1" "string_find_substring" (func $string_find_substring (param i32 i32 i32 i32 i32) (result i32)))
  (import "wasi_snapshot_preview1" "string_from_int" (func $string_from_int(param i32 i32 i32) (result i32)))

  (memory 1 1)
  (data (i32.const 48) "ouaimibaokaomibaobaoaibuaiouousuebaobao")
  (data (i32.const 24) "ebao")
  (func (export "_start")

  (drop (call $string_find_substring (i32.const 48) (i32.const 40) (i32.const 24) (i32.const 4) (i32.const 16)))

  (call $proc_exit (i32.const 0))
;;    (drop)
  )
)