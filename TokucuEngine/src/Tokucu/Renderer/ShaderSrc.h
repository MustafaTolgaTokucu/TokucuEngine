		#pragma once
	namespace Tokucu {
		///////////////////////////////////////////////////////////////////////////////////////////////
		//SHADER CODES
		///////////////////////////////////////////////////////////////////////////////////////////////
		class ShaderSrc 
		{
		public:
			std::string fragmentSrc = R"(
		
				#version 430 core
		
				uniform vec3 LightSourceColor;
				out vec4 FragColor;

				void main()
				{
				 FragColor = vec4(LightSourceColor,1.0f); // set all 4 vector values to 1.0
				}

				)";
			std::string vertexSrc = R"(
				#version 430 core

				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec3 a_Normal;
				layout(location = 2) in vec2 a_Texture; 
				layout (std140) uniform Matrices
				{
				    mat4 projection;
				    mat4 view;
					int numofLights;
				};
				uniform mat4 transform;
				out vec3 FragPos; 
				out vec3 Normal; 
				out vec3 v_Position;
				out vec2 TexCoords;
				flat out int lightNumber;
				void main() {
					lightNumber=numofLights;
					v_Position = a_Position;
					TexCoords = a_Texture;
					Normal = mat3(transpose(inverse(transform))) * a_Normal;  
					FragPos = vec3(transform * vec4(a_Position, 1.0));
					gl_Position = projection *view *transform *vec4(a_Position, 1.0);
				}
					)";
			std::string fragmentSrcTarget = R"(
				#version 430 core

				struct Material {
				sampler2D diffuse;
				sampler2D specular;
				sampler2D emission;
				float     shininess;
				};  

				struct SpotLight {
					vec3 position;// no longer necessary when using directional lights.
					vec3 direction;

					vec3 ambient;
					vec3 diffuse;
					vec3 specular;

					vec3 color;
					float intensity;

					float constant;
					float linear;
					float quadratic;

					float cutOff;//for spotlight
					float outerCutOff;
				};
				struct DirLight {
					vec3 direction;
		  
					vec3 ambient;
					vec3 diffuse;
					vec3 specular;
				};  
				uniform DirLight dirLight;
				uniform SpotLight spotlight; 
				uniform Material material;

				uniform sampler2D texture_diffuse1;
				uniform samplerCube skybox;

				struct LightAttributes
				{
					vec3 color;
					vec3 position;
					vec3 ambient;
					vec3 diffuse;
					vec3 specular;
					float constant;
					float linear;
					float quadratic;
				};

				layout(std430, binding = 1) buffer PointLightBuffer {
				  LightAttributes lightsArray[];  // Array of point lights
				};
				
				out vec4 FragColor;

				uniform vec3 viewPos;

				in vec3 FragPos;  
				in vec3 Normal; 
				in vec2 TexCoords;
				flat in int lightNumber;

				vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
				vec3 CalcPointLight(LightAttributes light, vec3 normal, vec3 fragPos, vec3 viewDir);  
				vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  

				void main(){

				vec3 norm = normalize(Normal);
				vec3 viewDir = normalize(viewPos - FragPos);

				vec3 reflectDir = normalize(FragPos - viewPos);
				vec3 R = reflect(reflectDir, norm);
				vec4 reflection=vec4(texture(skybox, R).rgb,1.0);

				//vec3 result = CalcDirLight(dirLight, norm, viewDir);
				vec3 result = vec3(0.0);
				vec4 color;

				// phase 2: Point lights
					for(int i = 0; i < 1; i++)
					result += CalcPointLight(lightsArray[0], norm, FragPos, viewDir); 
				
				// phase 3: Spot light
				//result += CalcSpotLight(spotlight, norm, FragPos, viewDir);   

				color = vec4(result,1.0f);
				//FragColor=mix(reflection,color,0.5);
				FragColor=vec4(result,1.0f);
				//FragColor=vec4(lightsArray[0].ambient*vec3(texture(texture_diffuse1, TexCoords))*lightsArray[0].color,1.0f);
				}

				vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
				{
					vec3 lightDir = normalize(-light.direction);
					// diffuse shading
					float diff = max(dot(normal, lightDir), 0.0);
					// specular shading
					vec3 reflectDir = reflect(-lightDir, normal);
					float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

					// combine results
					vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
					vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
					vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
					return (ambient + diffuse + specular);
				}  
				vec3 CalcPointLight(LightAttributes light, vec3 normal, vec3 fragPos, vec3 viewDir)
				{
					vec3 lightDir = normalize(light.position - fragPos);
					vec3 halfwayDir = normalize(lightDir + viewDir);
					float diff = max(dot(normal, lightDir), 0.0);
					
					// specular shading
					vec3 reflectDir = reflect(-lightDir, normal);
					//float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
					float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 256);
					
					// attenuation
					float distance    = length(light.position - fragPos);
					float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
					
					// combine results
					vec3 ambient  =  light.ambient*vec3(texture(material.diffuse, TexCoords))*light.color;
					vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords))*light.color;
					vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

					ambient  *= attenuation;
					diffuse  *= attenuation;
					specular *= attenuation;
					return (ambient + diffuse + specular);
				} 
				vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
				{
					// diffuse 
					vec3 lightDir = normalize(light.position - FragPos);
					float diff = max(dot(normal, lightDir), 0.0);
					// specular
					vec3 reflectDir = reflect(-lightDir, normal);  
					float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
					// spotlight (soft edges)
					float theta = dot(lightDir, normalize(-light.direction)); 
					float epsilon = (light.cutOff - light.outerCutOff);
					float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
		    
					// attenuation
					float distance    = length(light.position - FragPos);
					float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

					vec3 ambient  = light.ambient  *vec3(texture(material.diffuse, TexCoords))*light.color;
					vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords))*light.color;
					vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords))*light.color;

					ambient  *= attenuation; 
					diffuse   *= attenuation*intensity;
					specular *= attenuation*intensity;   
		        
					return (ambient + diffuse + specular);
						}
				)";

				
				std::string FramebufferVertex = R"(
		
				#version 430 core
				layout (location = 0) in vec2 aPos;
				layout (location = 1) in vec2 aTexCoords;

				out vec2 TexCoords;

				void main()
				{
					TexCoords = aTexCoords;
					gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
				}  

				)";
				std::string FramebufferFragment = R"(
		
				#version 430 core
				out vec4 FragColor;
				
				in vec2 TexCoords;
				
				uniform sampler2D screenTexture;
				
				//const float offset = 1.0 / 300.0;  

				void main()
				{
					///*vec2 offsets[9] = vec2[](
					//	vec2(-offset,  offset), // top-left
					//	vec2( 0.0f,    offset), // top-center
					//	vec2( offset,  offset), // top-right
					//	vec2(-offset,  0.0f),   // center-left
					//	vec2( 0.0f,    0.0f),   // center-center
					//	vec2( offset,  0.0f),   // center-right
					//	vec2(-offset, -offset), // bottom-left
					//	vec2( 0.0f,   -offset), // bottom-center
					//	vec2( offset, -offset)  // bottom-right    
					//);
					//
					//float kernel[9] = float[](
					//	-1, -1, -1,
					//	-1,  9, -1,
					//	-1, -1, -1
					//);
    				//
					//vec3 sampleTex[9];
					//for(int i = 0; i < 9; i++)
					//{
					//	sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
					//}
					//vec3 col = vec3(0.0);
					//for(int i = 0; i < 9; i++)
					//	col += sampleTex[i] * kernel[i];
    				//
					//FragColor = vec4(col, 1.0);*/
					FragColor = vec4(texture(screenTexture, TexCoords));

				}  

				)";
				std::string skyboxVertexShader = R"(
		
				#version 430 core
				layout (location = 0) in vec3 aPos;
				
				out vec3 TexCoords;
				
				layout (std140) uniform Matrices
				{
				    mat4 projection;
				   mat4 view;
					int numofLights;
				};
				uniform mat4 view2;
				void main()
				{
				    TexCoords = aPos;
					vec4 pos = projection *view2 * vec4(aPos, 1.0);
					gl_Position = pos.xyww;
				}  

				)";
				std::string skyboxFragmentShader = R"(
		
				#version 430 core
				out vec4 FragColor;
				
				in vec3 TexCoords;
				
				uniform samplerCube skybox;
				
				void main()
				{    
				    FragColor = texture(skybox, TexCoords)*0.5;
				}
				)";
		};
	}