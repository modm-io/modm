/**
*	Site-specific configuration settings for Highslide JS
*/
hs.showCredits = false;
hs.outlineType = 'custom';
hs.captionEval = 'this.a.title';
hs.registerOverlay({
	html: '<div class="close-simple-white" onclick="return hs.close(this)" title="Close"></div>',
	position: 'top right',
	useOnHtml: true,
	fade: 2 // fading the semi-transparent overlay looks bad in IE
});

