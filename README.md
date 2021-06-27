## My Game Programming in C++

・マルチメディアライブラリSDLを用いてWindows上で動作するアプリ。

・サウンド用ライブラリFMOD、グラフィックライブラリOpenGLを用いており、  
・上記の他には、ファイル記述を扱うRapid JSONライブラリ、std::vector等のC++の標準的なライブラリを
　用いているが、  
 　それ以外は自身でコーディングしている。

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
