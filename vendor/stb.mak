LIB_OBJS += build/obj/vendor/stb/stb_ds.o

ALL_DIRS += build/obj/vendor/stb

build/obj/vendor/stb/stb_ds.o: vendor/stb/stb_ds.h | build/obj/vendor/stb
	$(CCACHE) $(CC) $(CFLAGS) -x c -DSTB_DS_IMPLEMENTATION $< -c -o $@
