#!/bin/bash
# Написать скрипт, который подсчитывает количество файлов в каталоге с учетом подкатологов.

ls -lR | grep ^- | wc -l