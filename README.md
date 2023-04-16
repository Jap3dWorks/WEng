# Storage Resolver 
## Introduction

 * DBFileType
   - id 
   - Code Alembic | USD | jpg |
   - Extensions -> .ma, .mb | .jpg, .png | 
   - Version v000 | v001
   - [Software]

 * DBAbstractMetadata
   - id -> Unique
   - PubDate
   - Version -> up version
   - Extension -> .abc, .usd, .usda, .usdc, .exr, .jpg
   - idmd5
   - PubDate
   - Requests 

 * DBAsset
   - Code -> CHAR PROP ELEM FFXX CAMERA (enum)
   - Name -> juan
   - idmd5

 * DBShot(file)
   - Episode
   - Sequence
   - Shot
   - Step anm
   - Software
   - idmd5 -> name idmd5
   - Version 15

 [shot] -> [Ouputs]
 * DBShotAsset -> CACHES, MODELOS, RENDER 
   - SHOT
   - ASSET
   - IDENTIFIER
   - WORLD_MATRIX
   - EXTENSION
   - idmd5

 * DBFILE
   - id
   - md5 -> unique 
   - Name
   - FileType
  
