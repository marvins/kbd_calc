# Math Canvas Refactor Ideas

> Throw-away brainstorm — no code changed. See `math_canvas.cpp` for current implementation.

---

## Current Pain Points

1. **`draw_box` lambda does too many things** — dispatches on `Box_Kind`, constructs LVGL
   descriptors, computes geometry, and calls draw functions, all in one block.
   Adding a new box type (e.g. SQRT visual, radical sign) requires editing a growing lambda.

2. **`offset_x / offset_y` parameters are vestigial** — `layout()` already sets absolute
   positions on every node. The offsets are always `0, 0` at the call site and just add
   noise to the recursive signature.

3. **`lv_area_t` construction is copy-pasted three times** — the `{x, y, x+w, y+h}` pattern
   with identical `static_cast<int32_t>` noise appears for atom label, placeholder rect,
   and fraction bar. A one-liner helper kills all three.

4. **`std::function` for recursion has overhead** — `std::function` type-erases the lambda,
   which forces a heap allocation per call. On embedded this matters. A small local struct
   with `operator()` that captures `layer` by reference is zero-cost.

5. **`width` and `height` are passed in but could be queried from the canvas** —
   `lv_obj_get_width(canvas)` / `lv_obj_get_height(canvas)` already know these. The caller
   (`LCD_Section::refresh`) shouldn't need to track them separately.

6. **Result string rendering is not part of the layout tree** — it's drawn directly with
   hardcoded `width/2, height/2` offsets, unrelated to the typeset expression. As the
   display grows (history, cursor, etc.) this will collide. It should become a proper
   layout region.

---

## Proposed Directions

### A. Extract per-kind draw functions (most impactful)

Replace the monolithic lambda body with named static helpers:

```cpp
static void draw_atom      (lv_layer_t&, const Layout_Box&);
static void draw_placeholder(lv_layer_t&, const Layout_Box&);
static void draw_fraction_bar(lv_layer_t&, const Layout_Box&, int x, int y);
```

The lambda becomes a clean dispatcher:

```cpp
switch (b.kind) {
    case Box_Kind::ATOM:     b.text.empty() ? draw_placeholder(layer, b)
                                            : draw_atom(layer, b);       break;
    case Box_Kind::FRACTION: draw_fraction_bar(layer, b, x, y);          break;
    default: break;
}
for (const auto& child : b.children) draw_box(child);
```

Each helper is independently testable and easy to extend.

### B. `make_area` inline helper

```cpp
static lv_area_t make_area(int x, int y, int w, int h) {
    return { static_cast<int32_t>(x),     static_cast<int32_t>(y),
             static_cast<int32_t>(x + w), static_cast<int32_t>(y + h) };
}
```

Removes all repeated cast noise.

### C. Local recursive struct instead of `std::function`

```cpp
struct Box_Drawer {
    lv_layer_t& layer;
    void operator()(const Layout_Box& b) const {
        // dispatch ...
        for (const auto& child : b.children) (*this)(child);
    }
};
Box_Drawer{layer}(box);
```

Zero overhead, cleaner capture, no heap allocation.

### D. Remove `offset_x / offset_y`

Since `layout()` writes absolute positions, the draw traversal only ever needs `b.pos.x`
and `b.pos.y` directly. Drop the extra parameters entirely.

### E. Derive canvas size internally

```cpp
const int width  = lv_obj_get_width(canvas);
const int height = lv_obj_get_height(canvas);
```

Remove `width` and `height` from the `draw_math_to_canvas` signature. The header and all
callers simplify.

### F. Longer term: Visitor pattern on `Layout_Box`

If the number of box kinds grows significantly, a proper Visitor (or `std::variant` +
`std::visit`) would make it impossible to forget handling a new kind — the compiler
enforces exhaustive dispatch. Overkill for the current 6 kinds but worth noting.

---

## Priority Order (if/when implemented)

1. `make_area` helper — trivial, zero risk
2. Remove `offset_x / offset_y` — trivial, simplifies signature
3. Local struct instead of `std::function` — straightforward, embedded-friendly
4. Per-kind draw functions — most impact on readability and extensibility
5. Remove `width`/`height` from signature — small, tidy
6. Result string into layout tree — larger, deferred until display design is settled
