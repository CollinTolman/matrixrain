uniform sampler2D in_FontTexture;
uniform sampler2D in_MaskTexture;
uniform float in_ScreenWidth, in_ScreenHeight;

void main()
{
	float scanlineFactor = int(gl_FragCoord.y) % 2 == 0 ? 1.0 : 0.2;
	vec2 wc = gl_FragCoord.xy / vec2(in_ScreenWidth, in_ScreenHeight);
	vec4 color = texture2D(in_MaskTexture, vec2(wc.x, 1 - wc.y));
	gl_FragColor = scanlineFactor * color * gl_Color * texture2D(in_FontTexture, gl_TexCoord[0].xy);
}
