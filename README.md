Seam Carving
============
1.Introduction
==========================================================================================================================
Seam carving is a content aware image retarget technology, it can do 4 main job: changing aspect ratio, enlarging, content
amplification and object removing. the main idea is to find an optimum seam bease some pixel energy fucntion.

In this project I implement some basic algorithm, they are:
  .vertical seam carving
  .horizontal seam carving
  .mannuly order seam carving
  .content amplification
  
I wish somebody, andybody could make some improvement. such as using forward enery to reduce artificial, dynamic programming
to find optimum seam order, add image enlarging and object removing.

2.Result
==========================================================================================================================
 vertical seam carving
 horizontal seam carving
 mannuly order seam carving
 content amplification
 
3.Issue
==========================================================================================================================
When reduce image too much, more than 50% it's original size, some artificial occur. 
another issue is time consuming with big image, probbaly parallel alogrithm will help.