ba-elf-objdump -d     BK3266_DM_Full_Func_designkit.elf    > BK3266_DM_Full_Func_designkit.dmp
ba-elf-nm             BK3266_DM_Full_Func_designkit.elf    > BK3266_DM_Full_Func_designkit.map

#ba-elf-readelf.exe    BK3266_DM_Full_Func_designkit.elf -a > 0_section_information.txt
#ba-elf-nm -r          BK3266_DM_Full_Func_designkit.elf    > 1_reverse_sort_symbol.txt
#ba-elf-nm --size-sort BK3266_DM_Full_Func_designkit.elf    > 2_symbol_by_size.txt
#ba-elf-nm -p          BK3266_DM_Full_Func_designkit.elf    > 3_no_sort_symbol.txt
#ba-elf-nm -n          BK3266_DM_Full_Func_designkit.elf    > 4_numeric_sort_symbol.txt
#ba-elf-nm -l          BK3266_DM_Full_Func_designkit.elf    > 5_symbol_and_line_number.txt
#ba-elf-nm -g          BK3266_DM_Full_Func_designkit.elf    > 6_external_symbol.txt
#ba-elf-nm -a          BK3266_DM_Full_Func_designkit.elf    > 7_debug_symbol.txt
#ba-elf-nm -u          BK3266_DM_Full_Func_designkit.elf    > 8_undefined_symbol.txt
#ba-elf-nm -S          BK3266_DM_Full_Func_designkit.elf    > 9_print_size_defined_symbol.txt

ba-elf-objcopy -O binary BK3266_DM_Full_Func_designkit.elf  BK3266_DM_Full_Func_designkit.bin

./encrypt BK3266_DM_Full_Func_designkit.bin 00000000
