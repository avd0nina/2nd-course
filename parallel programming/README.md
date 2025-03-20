# Как зайти на сервер

1. Сгенерируйте ключ
```bash
ssh-keygen -t rsa -b 4096 -C "your_name@g.nsu.ru"
```
2.
```bash
chmod 600 ~/.ssh/id_rsa
```
3. 
```bash
eval "$(ssh-agent -s)"
```
4. 
```bash
ssh-add ~/.ssh/id_rsa
```
5. 
```bash
mv Users/.../lin_priv ~/.ssh/
```
6.
```bash
chmod 600 ~/.ssh/lin_priv
```
7. 
```bash
eval "$(ssh-agent -s)"
```
8. 
```bash
ssh-add ~/.ssh/lin_priv
```
9. 
```bash
ssh -i ~/.ssh/lin_priv -p 2222 oppN@84.237.52.21
```
10. f

