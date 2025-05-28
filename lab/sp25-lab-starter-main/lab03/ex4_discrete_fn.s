.data
function_values:
    .word 6,61,17,-38,19,42,5
.text
.globl f # this allows other files to find the function f

# f takes in two arguments:
# a0 is the value we want to evaluate f at
# a1 is the address of the "output" array (read the lab spec for more information).
# The return value should be stored in a0
f:
    # Your code here
    addi t0,a0,3
    slli t1,t0,2
    
    # This is how you return from a function. You'll learn more about this later.
    # This should be the last line in your program.
    add t3,t0,t1
    add t4,a1,t1
    lw t5,0(t4)
    mv a0,t5
    ret
 
