[//]: # (Abdussamet ERSOYLU // 2D Map Rendering and Algorithms)

# 2D Map Rendering and Algorithms 


## NOTE THAT THIS IS STILL EARLY ALPHA AND YOU SHOULDN'T TRY USING THIS IN ANY PROJECT RIGHT NOW. 
### *The folder "map-dev" is intentional and you should avoid this folder if you are aiming to use Gorgon. I'll probably be pushing uploads when a stage is complete.*

### Beginning of the project 
#### v0.01 Stage 1 
The project started with the Matrix class 
I'm using the CRTP pattern to build this module. 

Note: "###" shows the stage I'm at. 

Expected stages; 
* Stage 1 -> Developing the base structures needed by this module. 
  * Stage 1.1 -> Developing Matrix class. 
  * Stage 1.2 -> Develop base map class. ###
    * Stage 1.2.1 -> Develop an example map type for the demo. 
  * Stage 1.3 -> Develop base layer class. 
    * Stage 1.3.1 -> Develop an example layer type for the demo. 
  * Stage 1.4 -> Develop base object class. 
    * Stage 1.4.1 Develop an example object type for the demo. 
  * Stage 1.5 -> Build a working demo standard tile map with Tiled. 
  * Stage 1.6 -> Implement other types for demo. 
  * Stage 1.7 -> Build a working demo of different tile styles with Tiled.
  * Stage 1.8 -> Release the first update. 
* Stage 2 -> Implement a generic translating system for this module. 
  * Stage 2.??? Other stages are yet to be decided. 

### Change log: 
## v0.01 Stage 1.1 
Added base matrix, along with integer and float versions. 

Class definition; 
```c++
    class IMatrix : public Base_Matrix<IMatrix, int>;
    class FMatrix : public Base_Matrix<FMatrix, float>;  
```

The class has a multiplication function and necessary conversions. 

Usage: 
```c++
    // Choose a matrix type; 
    // for instance IMatrix
    IMatrix matrix = {(matrix_t<int>)
        {
            {1,2,3},
            {4,5,6},
            {7,8,9}
        }
    };
    
    // for instance FMatrix
    FMatrix matrix = {(matrix_t<float>)
        {
            {1.1, 2.2, 3.3},
            {4.4, 5.5, 6.6},
            {7.7, 8.8, 9.9}
        }
    };
```
### Why C style cast is used? 
The reason behind this is ambiguous constructor calls. There are two viable constructors for this kind of initializing. 
1. Integer to Matrix
2. Float to Matrix

### Why float to IMatrix (or integer to FMatrix)? 
The main reason is to let the user init constructor at the expense of losing some data but still giving them freedom to this. This method by default doesn't round. But can be implemented with rounding. 

### **//TODO: DOCUMENT THIS CLASS** 

Matrix classes will be used for transformations. 

## v0.01 Stage 1.2 
Started to develop the first tile map system. Once done will implement other map styles. 

*Stage is not done yet.*

