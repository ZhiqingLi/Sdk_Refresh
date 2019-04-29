echo "Post_steps.bat..."

ba-elf-objdump -d BK3251S_designkit.elf > BK3251S_designkit.dmp
ba-elf-nm         BK3251S_designkit.elf > BK3251S_designkit.map

echo "generate  flash data bin..."
echo "End Post_steps.bat..."
