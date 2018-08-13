MONGOOSEMM_OBJ= \
  src/mongoosemm/mongoosemm.o

MONGOOSEMM_LIB=$(LIB_DIR)/libmongoosemm.a

$(MONGOOSEMM_LIB): $(MONGOOSEMM_OBJ)
	$(AR) rcs $@ $<

src/mongoosemm/%.o: src/mongoosemm/%.cpp
	$(CXX) -c -o $@ -fPIC $(CFLAGS) $(CXXFLAGS) $<
	@sh ./depends.sh depends.Mk $<
