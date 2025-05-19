from locust import HttpUser, task, between

class APILoadTestUser(HttpUser):
    # 设置请求间隔时间（秒）
    wait_time = between(1, 3)
    
    # 测试服务器地址
    host = "http://172.29.116.225:8080"
    
    @task(3)  # 权重为3，表示执行频率是其他任务的3倍
    def get_index(self):
        """测试首页GET请求"""
        self.client.get("/")
        
    @task(2)
    def get_api_data(self):
        """测试API接口GET请求"""
        self.client.get("/api/data")
        
    @task(1)
    def post_form(self):
        """测试表单提交POST请求"""
        data = {"username": "test", "password": "123456"}
        self.client.post("/login", json=data)
        
if __name__ == "__main__":
    import os
    os.system("locust -f pressure_test.py")