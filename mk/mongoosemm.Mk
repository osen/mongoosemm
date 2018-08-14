MONGOOSEMM_OBJ= \
  src/mongoosemm/mongoosemm.o \
  src/mongoosemm/mongoose.o

MONGOOSEMM_LIB=$(LIB_DIR)/libmongoosemm.a

$(MONGOOSEMM_LIB): $(MONGOOSEMM_OBJ)
	$(AR) rcs $@ $(MONGOOSEMM_OBJ)

src/mongoosemm/%.o: src/mongoosemm/%.cpp
	$(CXX) -c -o $@ -fPIC $(CFLAGS) $(CXXFLAGS) $<
	@sh ./depends.sh depends.Mk $<

src/mongoosemm/%.o: src/mongoosemm/%.c
	$(CC) -c -o $@ -fPIC $(CFLAGS) $<
	@sh ./depends.sh depends.Mk $<
