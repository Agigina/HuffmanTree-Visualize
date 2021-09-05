from flask import Flask,render_template,request,flash, redirect, url_for,send_from_directory
from flask_wtf import FlaskForm
from wtforms import SubmitField, StringField, PasswordField
from flask_sqlalchemy import SQLAlchemy
from flask import current_app
from flask_login import UserMixin, LoginManager,AnonymousUserMixin,login_user, logout_user, login_required, current_user
import hashlib
import os
import shutil
from fromKey import FromKey

app=Flask(__name__)

db=SQLAlchemy(app)

app.secret_key='password'

app.config['SQLALCHEMY_DATABASE_URI']='mysql+pymysql://root:yibo83@127.0.0.1:3306/ds_assign?charset=utf8'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS']=False
app.config['SQLALCHEMY_ECHO']=True

class User(UserMixin, db.Model):
    # 没有昵称，只有qq号和密码
    id = db.Column(db.Integer, primary_key=True)
    email = db.Column(db.String(254), unique=True, nullable=False)
    password = db.Column(db.String(128))
    email_hash = db.Column(db.String(128))

    def __init__(self, **kwargs):
        super(User, self).__init__(**kwargs)
        self.get_email_hash()

    def get_email_hash(self):
        if self.email is not None and self.email_hash is None:
            self.email_hash = hashlib.md5(self.email.encode(
                'utf-8')).hexdigest()  # encode for py23 compatible

    @property
    def gravatar(self):
        if self.id <= 40:
            return 'https://gravatar.com/avatar/%s?d=monsterid' % self.email_hash
        else:
            return 'https://q4.qlogo.cn/g?b=qq&nk={qqnumber}&s=140'.format(qqnumber=self.email)


@app.route('/', methods=["GET", "POST"])
def login():
    # print("aaa")
    if request.method == "GET":
        # return render_template('calm-breeze-login-screen/dist/index.html')
        print("get")
    if request.method == "POST":
        username = request.form['username']
        password = request.form['password']
        # print(username,password)
        if not all ([username,password]):
            flash(u"参数不完整")
        return redirect(url_for('welcomes'))
        # user = User.query.filter_by(email=username).first()
        # print("ss",type(int(username)),int(username),user)
        # if user is not None:
        #     print("not none")
        #     if user.get_password(password):
        #         # login_user(user)
        #         return redirect(url_for('welcome'))
        # else:
        #     user = User(email=username, password=password)
        #     db.session.add(user)
        #     db.session.commit()
        #     # login_user(user)
        #     return redirect(url_for('welcomes'))
        
    return render_template('calm-breeze-login-screen/dist/index.html')

@app.route('/welcome')
def welcomes():
    return render_template('menu/index.html')

@app.route('/choice1', methods=["GET", "POST"])
def choice1():
    if request.method == "POST":
        # f=request.files['myfiles']
        # f.save(f.filename)
        # print(f.filename)
        keyvalue = request.form['keyvalue']
        key=keyvalue.split(' ')
        i=0
        files=open("KeyValue.log",'w')
        for item in key:
            if i%2==0:
                print(ord(item),end=" ",file=files)
            else:
                print(item,end=" ",file=files)
            if i%2==1:
                print('\n',end="",file=files)
            i+=1
        files.close()
        FromKey()
        shutil.copy("Total.gv.png","static/Total.png")
        return render_template('choices/input-box/result.html')
        # 保存图像，然后展示图像，return一个render00
        # 然后把encode和decode结果待下载
    # 如上
    return render_template('choices/input-box/index.html')

@app.route('/choice2', methods=["GET", "POST"])
def choice2():
    if request.method == "POST":
        f=request.files['myfiles']
        f.save(f.filename)
        print(f.filename)
        main = "Huffman.exe"
        order="encode"
        path=str(f.filename)
        final=main+r" "+order+r" "+path
        print(final)
        r_v = os.system(final) 
        
        order="decode"
        path="result.log"
        final=main+r" "+order+r" "+path
        print(final)
        r_v = os.system(final)
        # 原来：result.txt     压缩：result.log     解压后：result1.log
        # 调用fromkey   
        # 先显示下载界面，然后显示Totol.gv.png
        FromKey()
        shutil.copy("Total.gv.png","static/Total.png")
        return render_template('choices/upload/download.html')
        # 保存图像，然后展示图像，return一个render
        # 然后把encode和decode结果待下载
    return render_template('choices/upload/index.html')

@app.route('/result', methods=["GET", "POST"])
def test():
    return render_template('choices/upload/download.html')

@app.route('/download/<filename>', methods=["GET", "POST"])
def download(filename):
    return send_from_directory("",filename, as_attachment=True)

@app.route('/choice3', methods=["GET", "POST"])
def choice3():
    # 获取输入，保存成一个txt
    if request.method == "POST":
        # f=request.files['myfiles']
        # f.save(f.filename)
        # print(f.filename)
        keyvalue = request.form['keyvalue']
        files=open("result.txt",'w')
        print(keyvalue,file=files)
        files.close()
        main = "Huffman.exe"
        order="encode"
        path="result.txt"
        final=main+r" "+order+r" "+path
        print(final)
        r_v = os.system(final) 
        
        order="decode"
        path="result.log"
        final=main+r" "+order+r" "+path
        print(final)
        r_v = os.system(final)
        # 原来：result.txt     压缩：result.log     解压后：result1.log
        # 调用fromkey   
        # 先显示下载界面，然后显示Totol.gv.png
        FromKey()
        shutil.copy("Total.gv.png","static/Total.png")
        return render_template('choices/upload/download.html')
        # 保存图像，然后展示图像，return一个render
        # 然后把encode和decode结果待下载
    # 如上
    return render_template('choices/input-box/index1.html')

if __name__== "__main__":
    app.run()