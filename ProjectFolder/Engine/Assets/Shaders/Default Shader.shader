!�  
   ��uK�#Y'��X��                	                                    `      0'                                                                            2      6                                                       ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����    ����                                                        ��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       0'          �u��69���u��69�� 0�|���                                                                                                                                            	              R�                          ����    ������������                                      �      ����������������������������                                   
          \�                          ����    ������������                                     �       ����������������������������                               
             ^�                         ����    ������������                                     �   ����    ������������������������                               
   "          \�                          ����    ������������                                     �      ����������������������������                                  -          \�                          ����    ������������&                                     �      ����������������������������                                                                                                                                                                                                                                                                                       baseColor model_matrix ourTexture projection view                       Q�                        	                 P�                     position texCoord                         P�                          ��������   	          R�                      !   ��������             R�                          ��������TexCoord ourColor gl_Position                                               color                   ����Q�                      	             ����P�                   position texCoord                   ����P�                      	             ����R�                   TexCoord ourColor                       R�                    color    !!NVvp5.0
OPTION NV_internal;
OPTION NV_bindless_texture;
PARAM c[13] = { program.local[0..12] };
ATTRIB vertex_attrib[] = { vertex.attrib[0..1] };
OUTPUT result_attrib[] = { result.attrib[0..1] };
TEMP R0, R1, R2, R3;
TEMP T;
MUL.F32 R0, c[9], c[4].y;
MAD.F32 R0, c[8], c[4].x, R0;
MAD.F32 R1, c[10], c[4].z, R0;
MUL.F32 R0, c[9], c[5].y;
MUL.F32 R2, vertex.attrib[0].y, c[1];
MAD.F32 R0, c[8], c[5].x, R0;
MAD.F32 R2, vertex.attrib[0].x, c[0], R2;
MAD.F32 R2, vertex.attrib[0].z, c[2], R2;
MAD.F32 R0, c[10], c[5].z, R0;
ADD.F32 R3, R2, c[3];
MAD.F32 R0, c[11], c[5].w, R0;
MUL.F32 R0, R3.y, R0;
MAD.F32 R1, c[11], c[4].w, R1;
MAD.F32 R2, R3.x, R1, R0;
MUL.F32 R0, c[9], c[6].y;
MUL.F32 R1, c[7].y, c[9];
MAD.F32 R0, c[8], c[6].x, R0;
MAD.F32 R1, c[7].x, c[8], R1;
MAD.F32 R0, c[10], c[6].z, R0;
MAD.F32 R1, c[7].z, c[10], R1;
MAD.F32 R0, c[11], c[6].w, R0;
MAD.F32 R0, R3.z, R0, R2;
MAD.F32 R1, c[7].w, c[11], R1;
MAD.F32 result.position, R3.w, R1, R0;
MOV.F result.attrib[1], c[12];
MOV.F result.attrib[0].xy, vertex.attrib[1];
END
                                                                                                                                                                                                                                                                               ��������������������������������                                                                                                                                        ��������i  !!NVfp5.0
OPTION NV_internal;
OPTION NV_gpu_program_fp64;
OPTION NV_bindless_texture;
PARAM c[1] = { program.local[0] };
ATTRIB fragment_attrib[] = { fragment.attrib[0..1] };
TEMP R0;
LONG TEMP D0;
TEMP T;
OUTPUT result_color0 = result.color;
PK64.U D0.x, c[0];
TEX.F R0, fragment.attrib[0], handle(D0.x), 2D;
MUL.F32 result_color0, R0, fragment.attrib[1];
END
                                                                                                                                                  �������������������������������                                                                                                                                       ��������                        