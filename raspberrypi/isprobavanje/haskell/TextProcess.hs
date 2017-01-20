module TextProcess
  (processText
  ) where
  
import Data.List  
  
--someFunc :: IO ()
--someFunc = putStrLn "someFunc"

disassembleText :: String -> [String]
disassembleText text = concatMap words $ lines text

processText :: String -> [String]
processText words = nub $ sort $ disassembleText words