@rem Please install nkf32 command
@rem   SourceCode: http://sourceforge.jp/projects/nkf/
@rem   For Windows: http://www.vector.co.jp/soft/win95/util/se295331.html 

@rem Convert file encoding from UTF-8(without BOM) to UTF-8(with BOM)
for /f "tokens=*" %%d in ('"dir /b/s/a | findstr /e /i \.c"') do nkf32 -W8 -w8 --overwrite "%%d"
for /f "tokens=*" %%d in ('"dir /b/s/a | findstr /e /i \.h"') do nkf32 -W8 -w8 --overwrite "%%d"
for /f "tokens=*" %%d in ('"dir /b/s/a | findstr /e /i \.cpp"') do nkf32 -W8 -w8 --overwrite "%%d"
for /f "tokens=*" %%d in ('"dir /b/s/a | findstr /e /i \.txt.*"') do nkf32 -W8 -w8 --overwrite "%%d"
for /f "tokens=*" %%d in ('"dir /b/s/a | findstr /e /i \.md"') do nkf32 -W8 -w8 --overwrite "%%d"
