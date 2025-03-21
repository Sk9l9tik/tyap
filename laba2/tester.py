#!/usr/bin/env python3
def compare_files(file1, file2):
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        line_number = 1
        all_ok = True
        
        for line1, line2 in zip(f1, f2):
            if line1 != line2:
                print(f"Difference at line {line_number}:")
                print(f"File 1: {line1.strip()}")
                print(f"File 2: {line2.strip()}")
                print()
                all_ok = False
            line_number += 1
        
        # Проверка на случай, если файлы разной длины
        if f1.readline() or f2.readline():
            print("Files have different lengths.")
            all_ok = False
        
        if all_ok:
            print("OK")

# Пример использования
file1 = 'test.txt'
file2 = 'out.txt'
compare_files(file1, file2)