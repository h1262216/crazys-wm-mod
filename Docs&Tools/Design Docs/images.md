Starting with version 7.2, the new image selection algorithm will be used.
The algorithm works as follows: The girl pack provides an `images.xml` file, which
lists which images the game should you for which purpose. The file contains `<Image>` entries,
with each `<Image>` describing one possible image. The `<Image>` tag has the following attributes;
* **File** name of the file (in the same directory) to be used for this image. The same file can be used
in multiple images
* **Type** The primary image type of this image. 
* **Pregnant** Either "yes" or "no", to signify whether this is a pregnant variation of an image.
* **Fallback** (Optional, defaults to no) If this is `yes`, then the given image is a fallback image. That means
that if the game finds any image that fits just as good as this one for a particular task, this will be ignored.
The purpose of fallback images is to provide suboptimal image assignments (e.g. because adequate images are available)
and mark these as fallback, so that if an add-on pack provides better images, the game automatically ignores the 
fallback options.

