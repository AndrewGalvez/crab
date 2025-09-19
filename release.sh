make rebuild
mv game build/
zip -r crab2-linux.zip build/
gh release new $1
gh release upload $1 crab2-linux.zip
