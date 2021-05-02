# SearchEngine

Index is located at ```/home/cshome/d/dasoo/COSC431/Assignment1```
and comprises of:
- docIDs.txt;
- postings.txt;
- wordList.txt

To compile:
- ```Parser: gcc parser.c -o parser```
- ```Indexer: gcc indexer.c htable.c -o indexer```
- ```Searcher: gcc search.c htable.c -o search```

wsj.xml and indexes must be located in the same directory as parser/indexer/search engine.
