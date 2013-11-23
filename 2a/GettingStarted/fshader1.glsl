void main() 
{
	
	if (gl_FragCoord.x > 200 && gl_FragCoord.x < 600 &&
	    gl_FragCoord.y > 200 && gl_FragCoord.y < 600 ) {
	  gl_FragColor = vec4(0,1.0,0,1.0);
	} else {
	  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);  // set pixel color to red
    }
}