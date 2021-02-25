//
//  SCNShadable.h
//
//  Copyright (c) 2013-2017 Apple Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SCNMaterial;
@class SCNNode;
@class SCNRenderer;
@class SCNProgram;
@protocol MTLLibrary;
@protocol SCNProgramDelegate;
@protocol SCNShadable;

#if defined(SWIFT_SDK_OVERLAY2_SCENEKIT_EPOCH) && SWIFT_SDK_OVERLAY2_SCENEKIT_EPOCH >= 3
typedef NSString * SCNShaderModifierEntryPoint NS_STRING_ENUM;
#else
typedef NSString * SCNShaderModifierEntryPoint;
#endif

/*! @enum SCNBufferFrequency
 @abstract The frequency at which the custom program input should be updated.
 @discussion When the frequency is set to SCNBufferFrequencyPerFrame, the binding block is invoked once per frame.
 When the frequency is set to SCNBufferFrequencyPerNode, the binding block is invoked once per SCNNode.
 When the frequency is set to SCNBufferFrequencyPerShadable, the binding block is invoked once per SCNMaterial or SCNGeometry (depending on the object that owns the SCNProgram).
 */
typedef NS_ENUM(NSInteger, SCNBufferFrequency) {
    SCNBufferFrequencyPerFrame    = 0,
    SCNBufferFrequencyPerNode     = 1,
    SCNBufferFrequencyPerShadable = 2 // SCNMaterial or SCNGeometry
} API_AVAILABLE(macos(10.11), ios(9.0));

@protocol SCNBufferStream <NSObject>
- (void)writeBytes:(void *)bytes length:(NSUInteger)length;
@end

/*!
 @typedef SCNBufferBindingBlock
 @discussion Signature for the block to execute to bind or unbind a buffer.
 @param buffer The buffer to fill.
 @param node The rendered node.
 @param shadable The rendered shadable (geometry or material).
 @param renderer The renderer that is currently rendering the scene.
 */
typedef void (^SCNBufferBindingBlock)(id <SCNBufferStream> buffer, SCNNode *node, id <SCNShadable> shadable, SCNRenderer *renderer);

/*!
 @typedef SCNBindingBlock
 @discussion Signature for the block to execute to bind or unbind a uniform of an OpenGL or OpenGLES based program.
 @param programID The id of the program object to bind/unbind values for.
 @param location The location of the symbol within the program object to bind/unbind values for.
 @param renderedNode The node currently being rendered.
 @param renderer The renderer that is currently rendering the scene.
 */
typedef void (^SCNBindingBlock)(unsigned int programID, unsigned int location, SCNNode * _Nullable renderedNode, SCNRenderer *renderer);


/*!
 @protocol SCNShadable
 @abstract The SCNShadable protocol defines an object that is rendered with shaders. 
 */
@protocol SCNShadable <NSObject>
@optional

/*!
 @property program
 @abstract Specifies a custom program used to render the receiver.
 @discussion When a program is set, it overrides all the rendering parameters such as material settings and shaderModifiers.
 */
@property(nonatomic, retain, nullable) SCNProgram *program __WATCHOS_PROHIBITED;

/*!
 @method handleBindingOfSymbol:usingBlock:
 @abstract Sets the block to call at render time to bind the value for the specified symbol of the receiver's SCNProgram. This method has no effect for symbols declared in shader modifiers.
 @param symbol The name of the symbol to bind a value for.
 @param block The block to call to bind the specified symbol.
 @discussion This method can only be used with OpenGL and OpenGLES based programs.
 */
- (void)handleBindingOfSymbol:(NSString *)symbol usingBlock:(nullable SCNBindingBlock)block API_AVAILABLE(macos(10.9)) __WATCHOS_PROHIBITED;

/*!
 @method handleUnbindingOfSymbol:usingBlock:
 @abstract Sets the block to call at render time to unbind the value for the specified symbol of the receiver's SCNProgram. This method has no effect for symbols declared in shader modifiers.
 @param symbol The name of the symbol to unbind.
 @param block The block to call to unbind the specified symbol.
 @discussion This method can only be used with OpenGL and OpenGLES based programs.
 */
- (void)handleUnbindingOfSymbol:(NSString *)symbol usingBlock:(nullable SCNBindingBlock)block API_AVAILABLE(macos(10.9)) __WATCHOS_PROHIBITED;

/*!
 @property shaderModifiers
 @abstract Dictionary of shader modifiers snippets, targeting entry points. The valid keys are the entry points described in the "Shader Modifier Entry Point" constants. The values are the code snippets formatted as described below.
 @discussion Shader modifiers allow you to inject shader code in the standard shaders of SceneKit. This injection is allowed in few controlled entry points, allowing specific kind of tasks in specific context. Each modifier can operate on specific structures along with access to global uniforms, that could be the standard SceneKit uniforms or its own declared uniforms.
 
 The structure of a shader modifier is:
 
 | // Custom uniforms declarations of the form:
 | // for GLSL: [uniform type uniformName [= defaultValue]]
 | uniform float myGrayAmount = 3.0;
 |
 | //for Metal a pragma is required and arguments have the form [type name]
 | #pragma arguments
 | float myGrayAmount;
 |
 | In Metal, you can also tranfert varying values from the vertex shader (geometry modifier) to the fragment shader (surface/fragment modifier)
 | In one (or both) of the modifier, declare the varying values
 | #pragma varying
 | half3 myVec;
 |
 | Output them in the geometry modifier
 |  out.myVec = _geometry.normal.xyz * 0.5h + 0.5h;
 |
 | And use them in the fragment modifier for example
 | _output.color.rgb = saturate(in.myVec);
 |
 | // Optional global function definitions (for Metal: references to arguments from global functions are not supported).
 | float mySin(float t) {
 |    return sin(t);
 | }
 |
 | [#pragma transparent | opaque]
 | [#pragma body]
 |
 | // the shader modifier code snippet itself
 | vec3 myColor = vec3(myGrayAmount);
 | _output.color.rgb += myColor;
 
 The `#pragma body` directive
 Is only needed if you declared functions that must not be included in the shader code itself.
 
 The `#pragma transparent` directive
 Forces the rendering to be blended using the following equation:
 _output.color.rgb + (1 - _output.color.a) * dst.rgb;
 where `dst` represents the current fragment color. The rgb components must be premultiplied.
 
 The `#pragma opaque` directive
 Forces the rendering to be opaque. It then ignores the alpha component of the fragment.
 
 The SCNGeometry and SCNMaterial classes are key-value coding compliant classes, which means that you can set values for arbitrary keys. Even if the key `myAmplitude` is not a declared property of the class, you can still set a value for it.
 Declaring a `myAmplitude` uniform in the shader modifier makes SceneKit observe the reveiver's `myAmplitude` key. Any change to that key will make SceneKit bind the uniform with the new value.
 Common scalar types wrapped by NSValue are supported.
 Metal Only: 
 - MTLBuffer are also supported as values (introduced in macos(10.13), ios(11.0), tvos(11.0), watchos(4.0))
 - Complex data types (struct) declared in the metal shader are supported.
    - You can set them as a whole using NSData.
    - Or you can set individual struct members using the member name as key and a value compatible with the member type.
 
 Custom uniforms can be animated using explicit animations.
 
 The following GLSL types (and Objective-C counterparts) can be used to declare (and bind) custom uniforms:
 Example: uniform float myAmplitude = 0.5;
 
 GLSL types    | Objective-C types
 --------------------------------------
 int           | NSNumber, NSInteger, int
 float         | NSNumber, CGFloat, float, double
 vec2          | CGPoint
 vec3          | SCNVector3
 vec4          | SCNVector4
 mat4, mat44   | SCNMatrix4
 sampler2D     | SCNMaterialProperty
 samplerCube   | SCNMaterialProperty (with a cube map)
 
 The following prefixes are reserved by SceneKit and should not be used in custom names:
 u_
 a_
 v_
 
 SceneKit declares the following built-in uniforms:
 float u_time;                               // The current time, in seconds
 vec2  u_inverseResolution;                  // 1./screen size (available since iOS 9 and macOS 10.11)
 -------------------------------------------------------------------------------------
 mat4  u_modelTransform                      // See SCNModelTransform
 mat4  u_viewTransform                       // See SCNViewTransform
 mat4  u_projectionTransform                 // See SCNProjectionTransform
 mat4  u_normalTransform                     // See SCNNormalTransform
 mat4  u_modelViewTransform                  // See SCNModelViewTransform
 mat4  u_modelViewProjectionTransform        // See SCNModelViewProjectionTransform
 -------------------------------------------------------------------------------------
 mat4  u_inverseModelTransform               // The inverse matrix of u_modelTransform
 mat4  u_inverseViewTransform                // The inverse matrix of u_viewTransform
 mat4  u_inverseProjectionTransform          // The inverse matrix of u_projectionTransform
 mat4  u_inverseModelViewTransform           // The inverse matrix of u_modelViewTransform
 mat4  u_inverseModelViewProjectionTransform // The inverse matrix of u_modelViewProjectionTransform
 -------------------------------------------------------------------------------------
 mat2x3 u_boundingBox;                       // The bounding box of the current geometry, in model space, u_boundingBox[0].xyz and u_boundingBox[1].xyz being respectively the minimum and maximum corner of the box.
 
 When writing MetalSL shaders (and not going through the GLSL->MetalSL converter), the node dependant parameters are stored in a struct named scn_node (dropping the u_) and the frame dependant are stored in a struct named scn_frame.
 For example u_boundingBox -> scn_node.boundingBox
             u_modelViewTransform -> scn_node.modelViewTransform
             u_time -> scn_frame.time
 You can find the complete description of the scn_frame structure (SCNSceneBuffer) in the scn_metal file. The scn_node struct is generated at compile time and have no static definition.
 
 Shader modifiers can be used to tweak SceneKit rendering by adding custom code at the following entry points:
 1. vertex
 2. surface
 3. lighting
 4. fragment
 See below for a detailed explanation of these entry points and the context they provide.
 
 Shader modifiers can be written in GLSL or Metal. Metal shaders won't run on iOS 8 and macOS 10.10 or below.
 */
@property(nonatomic, copy, nullable) NSDictionary<SCNShaderModifierEntryPoint, NSString *> *shaderModifiers API_AVAILABLE(macos(10.9));

@end


/*!
 @group Semantic options
 @abstract Valid keys for the option parameter of setSemantic:forSymbol:options:
 */
FOUNDATION_EXTERN NSString * const SCNProgramMappingChannelKey __WATCHOS_PROHIBITED; /* This key is optional and may be used in association with the SCNGeometrySourceSemanticTexcoord semantic. It allows to associate a mapping channel from the geometry to a symbol from the program source code. The mapping channel allows to plug programs that work with multiple texture coordinates. The associated value must be a NSNumber(integer) greater than zero. */

/*!
 @class SCNProgram
 @abstract A SCNProgram lets you specify custom shaders to use when rendering materials.
 */
__WATCHOS_PROHIBITED
@interface SCNProgram : NSObject <NSCopying, NSSecureCoding>

/*!
 @method program
 @abstract Creates and initialize a program instance.
 */
+ (instancetype)program;

/*!
 @property vertexShader
 @abstract Determines the receiver's vertex shader.
 */
@property(nonatomic, copy, nullable) NSString *vertexShader;

/*!
 @property fragmentShader
 @abstract Determines the receiver's fragment shader.
 */
@property(nonatomic, copy, nullable) NSString *fragmentShader;

/*!
 @property tessellationControlShader
 @abstract Determines the receiver's tessellation control shader. Tessellation shaders require OpenGL Core Profile.
 */
@property(nonatomic, copy, nullable) NSString *tessellationControlShader API_AVAILABLE(macos(10.10)) API_UNAVAILABLE(ios, tvos, watchos);

/*!
 @property tessellationEvaluationShader
 @abstract Determines the receiver's tessellation evaluation shader. Tessellation shaders require OpenGL Core Profile.
 */
@property(nonatomic, copy, nullable) NSString *tessellationEvaluationShader API_AVAILABLE(macos(10.10)) API_UNAVAILABLE(ios, tvos, watchos);

/*!
 @property geometryShader
 @abstract Determines the receiver's geometry shader. Geometry shaders require OpenGL Core Profile.
 */
@property(nonatomic, copy, nullable) NSString *geometryShader API_AVAILABLE(macos(10.10)) API_UNAVAILABLE(ios, tvos, watchos);

/*!
 @property vertexFunctionName
 @abstract Determines the receiver's vertex function name.
 @discussion The name of the vertex function (for Metal programs).
 */
@property(nonatomic, copy, nullable) NSString *vertexFunctionName API_AVAILABLE(macos(10.11), ios(9.0));

/*!
 @property fragmentFunctionName
 @abstract Determines the receiver's fragment function name.
 @discussion The name of the fragment function (for Metal programs).
 */
@property(nonatomic, copy, nullable) NSString *fragmentFunctionName API_AVAILABLE(macos(10.11), ios(9.0));

/*!
 @method handleBindingOfBufferNamed:frequency:usingBlock:
 @abstract Sets the block to call at render time to bind the buffer of the specified symbol of the receiver's program.
 @param name The name of the buffer to bind.
 @param frequency The frequency at which the block has to be invoked. Can be per frame, per node or per geometry or material. See SCNBufferBindingBlock above.
 @param block The block that binds the buffer.
 @discussion This method can only be used with Metal based programs.
 */
- (void)handleBindingOfBufferNamed:(NSString *)name frequency:(SCNBufferFrequency)frequency usingBlock:(SCNBufferBindingBlock)block API_AVAILABLE(macos(10.11), ios(9.0));


/*!
 @property opaque
 @abstract Determines the receiver's fragment are opaque or not. Defaults to YES.
 */
@property(nonatomic, getter=isOpaque) BOOL opaque API_AVAILABLE(macos(10.10));

/*!
 @method setSemantic:forSymbol:options:
 @abstract Associates a SceneKit semantic to a symbol.
 @param semantic The SceneKit semantic to associate to the specified symbol.
 @param symbol A symbol from the program source code.
 @param options An optional dictionary. See the 'Semantic options' above.
 @discussion Associates semantics handled by the SceneKit runtime to a symbol from the program. Supported semantics are listed in SCNGeometry.h and SCNNode.h.
 */
- (void)setSemantic:(nullable NSString *)semantic forSymbol:(NSString *)symbol options:(nullable NSDictionary<NSString *, id> *)options;

/*!
 @method semanticForSymbol:
 @abstract Retrieves the SceneKit semantic associated to a symbol from the program source code.
 @param symbol A symbol from the program source code.
 */
- (nullable NSString *)semanticForSymbol:(NSString *)symbol;

/*!
 @property delegate
 @abstract Determines the receiver's delegate
 */
@property(nonatomic, assign, nullable) id <SCNProgramDelegate> delegate;

/*!
 @property library
 @abstract Specifies the metal library to use to locate the function names specified above. 
 @discussion If set to nil the default library is used. Defaults to nil.
 */
@property(nonatomic, retain, nullable) id <MTLLibrary> library API_AVAILABLE(macos(10.11), ios(9.0));

@end

/*!
 @protocol SCNProgramDelegate
 @abstract The SCNProgramDelegate protocol declares the methods that an instance of SCNProgram invokes to delegate the binding of parameters.
 */
__WATCHOS_PROHIBITED
@protocol SCNProgramDelegate <NSObject>

@optional
/*!
 @method program:withID:bindValueForSymbol:atLocation:renderer:
 @abstract Invoked on the delegate to let it bind program values and/or also bind associated graphics resources (such as textures).
 @param program The program to bind values for.
 @param symbol The name of the symbol to bind a value for.
 @param location The location of the symbol within the program object to be modified.
 @param programID The program object.
 @param renderer The renderer that is currently rendering the scene.
 */
- (BOOL)program:(SCNProgram *)program bindValueForSymbol:(NSString *)symbol atLocation:(unsigned int)location programID:(unsigned int)programID renderer:(SCNRenderer *)renderer API_DEPRECATED("Use -[SCNShadable handleBindingOfSymbol:usingBlock:] instead", macos(10.8, 10.10)) API_UNAVAILABLE(ios, tvos, watchos);

/*!
 @method program:withID:bindValueForSymbol:atLocation:renderer:
 @abstract Invoked on the delegate to let it unbind program values and/or also unbind associated graphic resources (such as textures).
 @param program The program to unbind values for.
 @param symbol The name of the symbol to unbind a value for.
 @param location The location of the symbol within the program object to be modified.
 @param programID The program object.
 @param renderer The renderer that is currently rendering the scene.
 */
- (void)program:(SCNProgram *)program unbindValueForSymbol:(NSString *)symbol atLocation:(unsigned int)location programID:(unsigned int)programID renderer:(SCNRenderer *)renderer API_DEPRECATED("Use -[SCNShadable handleUnbindingOfSymbol:usingBlock:] instead", macos(10.8, 10.10)) API_UNAVAILABLE(ios, tvos, watchos);

/*!
 @method handleError
 @abstract Invoked on the delegate whenever a compilation error occurs.
 @discussion Error domain is SCNErrorDomain.
 @param program The program that generated a compilation error.
 @param error The compilation error.
 */
- (void)program:(SCNProgram *)program handleError:(NSError *)error;

/*!
 @method programIsOpaque
 @abstract The delegate should implement this mehod and return NO if the fragments generated by the program are not opaque.
 @param program The queried program.
 @discussion This is deprecated. Use SCNProgram's opaque property instead.
 */
- (BOOL)programIsOpaque:(SCNProgram *)program API_DEPRECATED("Use SCNProgram.opaque instead", macos(10.8, 10.10)) API_UNAVAILABLE(ios, tvos, watchos);

@end

/*!
 @group Shader Modifier Entry Point
 @abstract Entry points designing the insertion point of the shader code snippet of a shader modifiers dictionary.
 */

/*!
 @constant SCNShaderModifierEntryPointGeometry
 @abstract This is the entry point to operate on the geometry vertices, for example deforming them.
 @discussion It operates entirely in the vertex shader stage. It's input is the geometry structure:
 
 Structures available from this entry point:
 
 | struct SCNShaderGeometry {
 |    vec4 position;
 |    vec3 normal;
 |    vec4 tangent;
 |    vec4 color;
 |    vec2 texcoords[kSCNTexcoordCount];
 | } _geometry;
 |
 | Access: ReadWrite
 | Stages: Vertex shader only
 
 kSCNTexcoordCount is a constant integer set to the number of texture coordinates used.
 
 All the geometric fields (position, normal and tangent) are in model space. You can use one of the provided automatic uniforms
 such as u_modelTransform or u_modelViewTransform if you want to operate in another space (but the results must stay in the model space, otherwise remaining computations won't be correct).
 The texture coordinates are the raw values found in the mesh, they have not been transformed yet by their optional contentsTransform. The contentsTransform if any is applied after the geometry shader modifier.
 
 Example: Simple sinusoidal deformation
 
 uniform float Amplitude = 0.1
 _geometry.position.xyz += _geometry.normal * (Amplitude*_geometry.position.y*_geometry.position.x) * sin(u_time);
 */
FOUNDATION_EXTERN SCNShaderModifierEntryPoint const SCNShaderModifierEntryPointGeometry API_AVAILABLE(macos(10.9));

/*!
 @constant SCNShaderModifierEntryPointSurface
 @abstract This is the entry point to alter the surface representation of the material, before the lighting has taken place.
 
 Structures available from this entry point:
 
 | struct SCNShaderSurface {
 |    vec3 view;                // Direction from the point on the surface toward the camera (V)
 |    vec3 position;            // Position of the fragment
 |    vec3 normal;              // Normal of the fragment (N)
 |    vec3 geometryNormal;      // Geometric normal of the fragment (normal map is ignored)
 |    vec3 tangent;             // Tangent of the fragment
 |    vec3 bitangent;           // Bitangent of the fragment
 |    vec4 ambient;             // Ambient property of the fragment
 |    vec2 ambientTexcoord;     // Ambient texture coordinates
 |    vec4 diffuse;             // Diffuse property of the fragment. Alpha contains the opacity.
 |    vec2 diffuseTexcoord;     // Diffuse texture coordinates
 |    vec4 specular;            // Specular property of the fragment
 |    vec2 specularTexcoord;    // Specular texture coordinates
 |    vec4 emission;            // Emission property of the fragment
 |    vec2 emissionTexcoord;    // Emission texture coordinates
 |    vec4 multiply;            // Multiply property of the fragment
 |    vec2 multiplyTexcoord;    // Multiply texture coordinates
 |    vec4 transparent;         // Transparent property of the fragment
 |    vec2 transparentTexcoord; // Transparent texture coordinates
 |    vec4 reflective;          // Reflective property of the fragment
 |    float metalness;          // Metalness property of the fragment
 |    vec2 metalnessTexcoord;   // Metalness texture coordinates
 |    float roughness;          // Roughness property of the fragment
 |    vec2 roughnessTexcoord;   // Roughness texture coordinates
 |    float shininess;          // Shininess property of the fragment
 |    float fresnel;            // Fresnel property of the fragment
 |    float ambientOcclusion;   // Ambient occlusion term of the fragment
 | } _surface;
 |
 | Access: ReadWrite
 | Stages: Fragment shader only
 
 All geometric fields are in view space.
 All the other properties will be colors (texture have already been sampled at this stage) or floats. You can however do an extra sampling of standard textures if you want.
 In this case the naming pattern is u_<property>Texture. For example u_diffuseTexture or u_reflectiveTexture. Note that you have to be sure that the material do have a texture
 set for this property, otherwise you'll trigger a shader compilation error.
 
 Example: Procedural black and white stripes
 
 uniform float Scale = 12.0;
 uniform float Width = 0.25;
 uniform float Blend = 0.3;
 vec2 position = fract(_surface.diffuseTexcoord * Scale);
 float f1 = clamp(position.y / Blend, 0.0, 1.0);
 float f2 = clamp((position.y - Width) / Blend, 0.0, 1.0);
 f1 = f1 * (1.0 - f2);
 f1 = f1 * f1 * 2.0 * (3. * 2. * f1);
 _surface.diffuse = mix(vec4(1.0), vec4(0.0), f1);
 */
FOUNDATION_EXTERN SCNShaderModifierEntryPoint const SCNShaderModifierEntryPointSurface API_AVAILABLE(macos(10.9));

/*!
 @constant SCNShaderModifierEntryPointLightingModel
 @abstract This is the entry point to provide custom lighting equation. The fragment will be called for each active light
 of the scene and will need to accumulate lighting contribution for the vertex or the fragment in the _lightingContribution structure, using the light structure given.
 
 Structures available from the this entry point:
 
 | All the structures available from the SCNShaderModifierEntryPointSurface entry point
 |
 | Access: ReadOnly
 | Stages: Vertex shader and fragment shader
 
 | struct SCNShaderLightingContribution {
 |    vec3 ambient;
 |    vec3 diffuse;
 |    vec3 specular;
 | } _lightingContribution;
 |
 | Access: ReadWrite
 | Stages: Vertex shader and fragment shader
 
 | struct SCNShaderLight {
 |    vec4 intensity;
 |    vec3 direction; // Direction from the point on the surface toward the light (L)
 | } _light;
 |
 | Access: ReadOnly
 | Stages: Vertex shader and fragment shader
 
 Example: wrap diffuse lighting
 
 uniform float WrapFactor = 0.5;
 float dotProduct = (WrapFactor + max(0.0, dot(_surface.normal,_light.direction))) / (1 + WrapFactor);
 _lightingContribution.diffuse += (dotProduct * _light.intensity.rgb);
 vec3 halfVector = normalize(_light.direction + _surface.view);
 dotProduct = max(0.0, pow(max(0.0, dot(_surface.normal, halfVector)), _surface.shininess));
 _lightingContribution.specular += (dotProduct * _light.intensity.rgb);
 */
FOUNDATION_EXTERN SCNShaderModifierEntryPoint const SCNShaderModifierEntryPointLightingModel API_AVAILABLE(macos(10.9));

/*!
 @constant SCNShaderModifierEntryPointFragment
 @abstract This is the last entry point in the fragment shader, where you can alter the final color returned by the shader.
 @discussion You can alter the final color by reading and writing to the output color via the output structure below.
 
 Structures available from the this entry point:
 
 | All the structures available from the SCNShaderModifierEntryPointSurface entry point
 |
 | Access: ReadOnly
 | Stages: Fragment shader only
 
 | struct SCNShaderOutput {
 |    vec4 color;
 | } _output;
 |
 | Access: ReadWrite
 | Stages: Fragment shader only
 
 Example: inverse final color
 
 _output.color.rgb = vec3(1.0) - _output.color.rgb;
 */
FOUNDATION_EXTERN SCNShaderModifierEntryPoint const SCNShaderModifierEntryPointFragment API_AVAILABLE(macos(10.9));

NS_ASSUME_NONNULL_END
