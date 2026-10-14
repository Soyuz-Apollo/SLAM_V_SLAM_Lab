Image Processing and Computer Vision functions
==============================================

This document tries to explain the design decisions taken in composing
a set of image processing and vision functions.

The original set of IP and CV functions were implemented as C-style
functions (although in some cases templated).  Each function took a
set of parameters, usually an image, some modifiers such as thresholds
(possibly with default values) and a reference parameters (or
parameters) used for returning values.  The return type of the
function was always void or, in some cases, bool (used to indicate
success or failure).  

Use of reference parameters for return values means that a consistent
design can be applied.  When using the return values of functions,
there are often exceptions where we want to return more than one
object, and one also has to be careful about returning references to
temporary objects, or deal with the overheads of copying return values
onto and from the stack.

For example, image subsampling was implemented as a function which
took an image, a reference to a result image (which may or may not be
pre-allocated) and an optional factor for the subsampling (defaulting
to 2).  It was called as in the following examples:

	Subsample(im, res);     // subsample by default 2
	Subsample(im, res, 3);  // subsample by 3


The disadvantage of this approach is that as the number of modifiers
increases, the readability of the code decreases, and the appeal of
some class storage for the modifiers increases.  For this reason I
have changed the functions into classes, with each IP or CV function
implemented as a "function object".  That is, the function is
encapsulated in a class with the name of the function.  Each class
provides:

	-- storage for parameters
	-- a "Compute" method
	-- an overloaded "operator bool()" to enable the object to
	   queried for success or failure (although presently all
	   routines return true) 
	-- constructors which can be called to do the computation, and
	   appear exactly like an ordinary function call.

The first three points above mean that a call to a function can be
implemented as an instantiation of the object, followed by a call to
its Compute method.  For example:

	Subsample s(3);    // instantiate Subsample function object
			   // with factor=3 
	s.Compute(im,res); // now subsample im by 3 and put result in
			   // res 

This is nice if you want to apply the same set of modifying parameters
to a series of data for example.  However the provision of a set of
appropriate constructors (fourth point above) which also do the
computation means that a temporary class instantiation appears exactly
the same as a function call, viz:

	Subsample(im, res, 3);  // looks like a function but is
				// actually a temporary instantiation  
				// of a Subsample object which does
				// the computation required then
				// disappears, leaving the result of the
				// computation in res 

There is little or no efficiency overhead to pay for this.  The main
disadvantage is the additional complexity encountered by someone
trying to read the code, but it has a number of advantages which
(hopefully) outweigh this:

	-- large numbers of behaviour modifying parameters are more
	   easily managed
	-- the additional flexibility of having two ways to invoke the
	   computation
	-- it is now very easy to create an interface class which sets
	   sets parameters via a gui and then calls the computation
	-- parameters can be added in without changing the interface
	   (of course this could be done previously by overloading the
	   function)

So far, the only image processing operation whose function call has
changed is the thresholding operation, since the Threshold class is
templated on the type of the threshold value.  It therefore needs to
be called as follows:

        Threshold<float>(im,res,thresh_val);


