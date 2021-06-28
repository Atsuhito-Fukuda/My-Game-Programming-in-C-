## My Game Programming in C++

・マルチメディアライブラリSDLを用いてWindows上で動作するアプリ。

・また、サウンド用ライブラリFMOD、グラフィックライブラリOpenGLを用いており、  
・上記の他には、ファイル記述を扱うRapid JSONライブラリ、std::vector等のC++の標準的なライブラリを  
　用いているが、それ以外は自身でコーディング。
 
・サウンドをnamespace audioが、  
　グラフィックをnamespace gpuが、  
　衝突処理をnamespace actionが、  
　各オブジェクト固有のデータをnamespace objectが、  
　ファイル読み込み・データの動的生成をnamespace object::dataが、  
　カメラ処理をnamespace cameraが、  
　ゲーム全体の流れの処理をnamespace gameが、  
　コントローラの入力処理をnamespace game::controllerが、  
　(ゲームの内容記述をgame::GameSccriptクラスが、)  
　エラー処理をnamespace librarian::errorが、  
　担当しており、  
　その他、コンテナ、バイナリツリー、ベクトル・行列を提供する自作クラスの  
　ARRAY、IDmap、BinaryTree、Vector(、Tools)がある。  

・現段階では単純な内容を扱う。

・より詳しい紹介は各実装ファイルに。  

・Coding技術紹介資料_メンバとアクセス制御.pdfのデモンストレーション用に。

## コンパイル (Visual Studio)

SDL2, glew2, Simple OpenGL Image Library, RapidJSON, FMOD SoundSystem(FMOD Studio API, FMOD Core API)のヘッダ・ライブラリが必要。

(SDL2.lib, SDL2main.lib, SDL2_ttf.lib, SDL2_mixer.lib, SDL2_image.lib, glew32.lib, opengl32.lib, SOIL.lib, fmodstudioL_vc.lib, fmodL_vc.lib, 

SDL2.dll, SDL2_image.dll, libpng16-16.dll, glew32.dll, fmodL.dll, fmodstudioL.dll, zlib1.dll)

.slnファイルを開いてコンパイルしてください。

## 借用

/Assets/3D内のCube.png, earth.pngと/Assets/Audioは

https://github.com/gameprogcpp/code  
https://visibleearth.nasa.gov/images/73826/october-blue-marble-next-generation-w-topography-and-bathymetry/73835l

より拝借しています。
