ba-elf-objdump -d     bk326x_s_designkit.elf    > bk326x_s_designkit.dmp
ba-elf-nm             bk326x_s_designkit.elf    > bk326x_s_designkit.map

ba-elf-readelf        bk326x_s_designkit.elf -a > 0_section_information.txt
ba-elf-nm -r          bk326x_s_designkit.elf    > 1_reverse_sort_symbol.txt
ba-elf-nm --size-sort bk326x_s_designkit.elf    > 2_symbol_by_size.txt
ba-elf-nm -p          bk326x_s_designkit.elf    > 3_no_sort_symbol.txt
ba-elf-nm -n          bk326x_s_designkit.elf    > 4_numeric_sort_symbol.txt
ba-elf-nm -l          bk326x_s_designkit.elf    > 5_symbol_and_line_number.txt
ba-elf-nm -g          bk326x_s_designkit.elf    > 6_external_symbol.txt
ba-elf-nm -a          bk326x_s_designkit.elf    > 7_debug_symbol.txt
ba-elf-nm -u          bk326x_s_designkit.elf    > 8_undefined_symbol.txt
ba-elf-nm -S          bk326x_s_designkit.elf    > 9_print_size_defined_symbol.txt
