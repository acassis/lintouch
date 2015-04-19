CC=gcc -O3  -fPIC -Wall -W
#CFLAGS=-Wall -O3 -fPIC -I.\
CFLAGS=-Wall -pedantic -ggdb3 -O3 -fPIC
#LFLAGS=-shared -lefence
LFLAGS=-shared

PREFIX=/usr/local

OBJS =	aliascheck.o \
	attach.o \
	cell_geterror.o \
	cell_perror.o \
	cpf.o \
	cpf_data.o \
	defs.o \
	df1_close.o \
	df1_create_memory.o \
	df1_get_status.o \
	df1_open.o \
	df1_typed_read.o \
	df1_word_read.o \
	df1_word_write.o \
	df1_read_section_size.o \
        establish_connection.o \
	fill_header.o \
	get_backplane_data.o \
        get_device_data.o \
        get_dh_map.o \
	get_object_config.o \
	get_object_config_list.o \
	get_object_details.o \
	get_object_details_list.o \
	get_object_size.o \
	get_port_data.o \
	get_program_details.o \
	get_program_list.o \
	get_reply_status.o \
	get_remote_dh.o \
	get_struct_config.o \
	get_struct_details.o \
	get_struct_list.o \
	getsize.o \
	get_element_size.o \
	ioi.o \
	list_interfaces.o \
	list_ports.o \
	list_services.o \
	makepath.o \
	nameconv5.o \
	nop.o \
	object_bit_modify_word.o \
	object_bit_modify_byte.o \
	read_memory_value.o \
	read_multi_object_value.o \
	read_object_value.o \
	read_object_range_value.o \
	read_struct_element.o \
	read_tag.o \
	readdata.o \
	register_session.o \
	sendConnecteddata.o \
	sendRRdata.o \
	senddata.o \
	send_df1.o \
	settimeout.o \
	unconnected_send.o \
	unregister_session.o \
	who.o \
	write_object_value.o \
	write_object_range_value.o \
	write_tag.o

all:  libcell

*.o:	libcell.h

libcell: $(OBJS)
	ld -shared $(OBJS) -o libcell.so
	ar cr libcell.a $(OBJS)
	ranlib libcell.a

install:
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include

	cp -a libcell.so $(PREFIX)/lib
	cp -a libcell.a $(PREFIX)/lib
	cp -a libcell.h $(PREFIX)/include
	ldconfig

uninstall:
	rm -f $(PREFIX)/lib/libcell.so
	rm -f $(PREFIX)/lib/libcell.a
	rm -f $(PREFIX)/include/libcell.h
	ldconfig

clean:
	rm -f $(OBJS) libcell.a libcell.so *~

distclean: clean
