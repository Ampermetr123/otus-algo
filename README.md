# otus-algo

## Алгоритм сжатия RLE

- [программа](src)
- [тест функции](test)
- [тест программы на различных типах файлов](tests)


## Полученные результаты

  - data_test_archive.zip  - compressed size is 97%
  - data_test_audio.mp3  - compressed size is 97%
  - data_test_picture.jpg  - compressed size is 100%
  - data_test_text.txt  - compressed size is 97%

### Выводы

    Алгоритм ожидаемо показывает плохую степень сжатия. Повторов в исхдных данных очень мало.
    В таком виде RLE не имеет смысла.
    Чтобы применять алгоритм к реальным данным, необходимо
    использовать его совместно с преобразованием Барроуза-Виллера (BWT).
