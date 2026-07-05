# Comandi per creazione documentazione

1. Doxygen

```powershell
Set-Location C:\Users\franc\GitHub\Repositories\Sistemi_Operativi_Mobili\environment_monitor\docs\doxygen
doxygen .\doxyfile
```

2. Pandoc

```powershell
Set-Location C:\Users\franc\GitHub\Repositories\Sistemi_Operativi_Mobili\environment_monitor\docs\report
& "C:/Users/franc/AppData/Local/Pandoc/pandoc.exe" .\report.md -o .\report.docx
```
