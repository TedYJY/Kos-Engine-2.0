R"(
#version 460 core

in vec4 vColor;
in flat int vTexture;
in vec2 vTexCoords;
out vec4 fragColor;

uniform sampler2D textures[192]; // Subscript indicates max texture amount

void main()
{
    //fragColor = vColor; // simple white or whatever color is set
		if (vTexture < 200)
		{
			vec4 texColor = texture(textures[vTexture], vTexCoords) * vColor;

			if(texColor.a < 0.1)
			discard;
			fragColor = vec4(vec3(texColor.xyz),texColor.w);
		}
		else
		{
			fragColor = vColor;
		}
}
)"