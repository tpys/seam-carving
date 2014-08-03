Seam Carving
============
1.Introduction
==========================================================================================================================
Seam carving is a content aware image retarget technology, it can do 4 main job: changing aspect ratio, enlarging, content
amplification and object removing. the main idea is to find an optimum seam bease some pixel energy fucntion.

In this project I implement some basic algorithm, they are:
* vertical seam carving
* horizontal seam carving
* mannuly order seam carving
* content amplification
  
I wish somebody, andybody could make some improvement. such as using forward enery to reduce artificial, dynamic programming
to find optimum seam order, add image enlarging and object removing.

2.Result
==========================================================================================================================
 Original <br>
![ Original ](https://github.com/tpys/seam-carving/raw/master/bench.png)
 <br>  Vertical Seam Carving
![Vertical Seam Carving](https://github.com/tpys/seam-carving/raw/master/-v.png)
###
    Horizontal Seam Carving
![Horizontal Seam Carving](https://github.com/tpys/seam-carving/raw/master/-h.png)
![Mannuly Order Seam Carving](https://github.com/tpys/seam-carving/raw/master/-b.png)
![Content Amplification](https://github.com/tpys/seam-carving/raw/master/-a.png)

3.Issue
==========================================================================================================================
When reduce image too much, more than 50% it's original size, some artificial occur. 
another issue is time consuming with big image, probbaly parallel alogrithm will help.
