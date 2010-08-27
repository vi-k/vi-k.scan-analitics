/* GIMP RGBA C-Source image dump (red_mark.c) */

static const struct red_mark_st {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[16 * 16 * 4 + 1];
} red_mark = {
  16, 16, 4,
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\2\0\0\0\4\0\0\0\4"
  "\0\0\0\2\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\5\0\0\0\21\0\0\0\35\0\0\0$\0\0\0$\0\0\0\35\0\0\0\21"
  "\0\0\0\5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\12"
  "\0\0\0$\0\0\0""9\0\0\0C9\0\0U9\0\0U\0\0\0C\0\0\0""9\0\0\0$\0\0\0\12\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\12\0\0\0)\0\0\0E\235\0\0\206\350"
  "\0\0\323\363\0\0\346\363\0\0\346\350\0\0\323\235\0\0\206\0\0\0E\0\0\0)\0"
  "\0\0\12\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\5\0\0\0$\0\0\0E\314\0\0\256\377\0\0"
  "\377\376\0\0\377\377\0\0\377\377\0\0\377\376\0\0\377\377\0\0\377\314\0\0"
  "\256\0\0\0E\0\0\0$\0\0\0\5\0\0\0\0\0\0\0\0\0\0\0\21\0\0\0""9\235\0\0\206"
  "\377\0\0\377\376\0\0\377\377\0\0\377\377\0\0\376\377\0\0\376\376\0\0\377"
  "\376\0\0\377\377\0\0\377\235\0\0\206\0\0\0""9\0\0\0\21\0\0\0\0\0\0\0\2\0"
  "\0\0\36\0\0\0D\350\0\0\323\376\0\0\377\376\0\0\377\377\0\0\376\376\0\0\377"
  "\376\0\0\377\377\0\0\376\377\0\0\377\376\0\0\377\350\0\0\323\0\0\0D\0\0\0"
  "\36\0\0\0\2\0\0\0\4\0\0\0$8\0\0U\363\0\0\346\377\0\0\377\377\0\0\376\377"
  "\0\0\377\377\0\0\376\377\0\0\376\377\0\0\377\377\0\0\376\377\0\0\377\363"
  "\0\0\3468\0\0U\0\0\0$\0\0\0\4\0\0\0\4\0\0\0$8\0\0U\363\0\0\346\377\0\0\377"
  "\377\0\0\376\377\0\0\377\377\0\0\376\377\0\0\376\377\0\0\377\377\0\0\376"
  "\377\0\0\377\363\0\0\3469\0\0U\0\0\0$\0\0\0\4\0\0\0\2\0\0\0\36\0\0\0D\350"
  "\0\0\323\376\0\0\377\376\0\0\377\376\0\0\376\377\0\0\377\376\0\0\377\377"
  "\0\0\376\376\0\0\377\376\0\0\377\350\0\0\323\0\0\0D\0\0\0\36\0\0\0\2\0\0"
  "\0\0\0\0\0\21\0\0\0""9\235\0\0\206\377\0\0\377\377\0\0\377\377\0\0\377\377"
  "\0\0\376\377\0\0\376\376\0\0\377\377\0\0\377\377\0\0\377\235\0\0\206\0\0"
  "\0""9\0\0\0\21\0\0\0\0\0\0\0\0\0\0\0\5\0\0\0$\0\0\0E\315\0\0\256\377\0\0"
  "\377\376\0\0\377\377\0\0\377\377\0\0\377\376\0\0\377\377\0\0\377\315\0\0"
  "\256\0\0\0E\0\0\0$\0\0\0\5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\12\0\0\0)\0\0\0"
  "E\235\0\0\206\350\0\0\323\363\0\0\346\363\0\0\346\350\0\0\323\235\0\0\206"
  "\0\0\0E\0\0\0)\0\0\0\12\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\12"
  "\0\0\0$\0\0\0""9\0\0\0C8\0\0U9\0\0U\0\0\0C\0\0\0""9\0\0\0$\0\0\0\12\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\5\0\0\0\21\0\0\0"
  "\35\0\0\0$\0\0\0$\0\0\0\35\0\0\0\21\0\0\0\5\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\2\0\0\0\4\0\0\0"
  "\4\0\0\0\2\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
};

